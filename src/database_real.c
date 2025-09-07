/*
 * INGRES ChatBot - Real PostgreSQL Database Integration
 * Replaces the sample data with actual database connections
 */

#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

// Global database connection
static PGconn *db_conn = NULL;

// Database configuration
typedef struct {
    char host[256];
    char dbname[256];
    char user[256];
    char password[256];
    char port[16];
} DatabaseConfig;

static DatabaseConfig db_config = {
    .host = "localhost",
    .dbname = "ingres_groundwater",
    .user = "ingres_user",
    .password = "ingres_password",
    .port = "5432"
};

// Load configuration from environment or config file
static void load_db_config(void) {
    char *env_val;
    
    if ((env_val = getenv("DB_HOST")) != NULL) {
        strncpy(db_config.host, env_val, sizeof(db_config.host) - 1);
    }
    
    if ((env_val = getenv("DB_NAME")) != NULL) {
        strncpy(db_config.dbname, env_val, sizeof(db_config.dbname) - 1);
    }
    
    if ((env_val = getenv("DB_USER")) != NULL) {
        strncpy(db_config.user, env_val, sizeof(db_config.user) - 1);
    }
    
    if ((env_val = getenv("DB_PASSWORD")) != NULL) {
        strncpy(db_config.password, env_val, sizeof(db_config.password) - 1);
    }
    
    if ((env_val = getenv("DB_PORT")) != NULL) {
        strncpy(db_config.port, env_val, sizeof(db_config.port) - 1);
    }
}

// Initialize database connection
bool db_init(void) {
    load_db_config();
    
    // Build connection string
    char conninfo[1024];
    snprintf(conninfo, sizeof(conninfo),
             "host=%s dbname=%s user=%s password=%s port=%s",
             db_config.host, db_config.dbname, db_config.user, 
             db_config.password, db_config.port);
    
    // Connect to database
    db_conn = PQconnectdb(conninfo);
    
    if (PQstatus(db_conn) != CONNECTION_OK) {
        fprintf(stderr, "[DB ERROR] Connection failed: %s\n", PQerrorMessage(db_conn));
        PQfinish(db_conn);
        db_conn = NULL;
        return false;
    }
    
    printf("[DB] Connected to PostgreSQL database: %s\n", db_config.dbname);
    
    // Test the connection with a simple query
    PGresult *res = PQexec(db_conn, "SELECT COUNT(*) FROM groundwater_data;");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[DB ERROR] Test query failed: %s\n", PQerrorMessage(db_conn));
        PQclear(res);
        return false;
    }
    
    int record_count = atoi(PQgetvalue(res, 0, 0));
    printf("[DB] Database ready with %d groundwater records\n", record_count);
    
    PQclear(res);
    return true;
}

// Close database connection
void db_close(void) {
    if (db_conn) {
        PQfinish(db_conn);
        db_conn = NULL;
        printf("[DB] Database connection closed\n");
    }
}

// Execute query and return result
static PGresult* execute_query(const char* query) {
    if (!db_conn) {
        fprintf(stderr, "[DB ERROR] No database connection\n");
        return NULL;
    }
    
    PGresult *res = PQexec(db_conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[DB ERROR] Query failed: %s\nQuery: %s\n", 
                PQerrorMessage(db_conn), query);
        PQclear(res);
        return NULL;
    }
    
    return res;
}

// Convert PGresult row to GroundwaterData structure
static GroundwaterData pg_row_to_groundwater_data(PGresult *res, int row) {
    GroundwaterData data = {0};
    
    // Map database columns to structure fields
    strncpy(data.state, PQgetvalue(res, row, 0), sizeof(data.state) - 1);
    strncpy(data.district, PQgetvalue(res, row, 1), sizeof(data.district) - 1);
    strncpy(data.block, PQgetvalue(res, row, 2), sizeof(data.block) - 1);
    strncpy(data.category, PQgetvalue(res, row, 3), sizeof(data.category) - 1);
    
    data.annual_recharge = atof(PQgetvalue(res, row, 4));
    data.extractable_resource = atof(PQgetvalue(res, row, 5));
    data.annual_extraction = atof(PQgetvalue(res, row, 6));
    data.assessment_year = atoi(PQgetvalue(res, row, 7));
    
    return data;
}

// Query by location (state, district, block)
QueryResult* query_by_location(const char* state, const char* district, const char* block) {
    if (!state) return NULL;
    
    char query[1024];
    
    if (block && strlen(block) > 0) {
        // Query specific block
        snprintf(query, sizeof(query),
                "SELECT state, district, block, category, annual_recharge, "
                "extractable_resource, annual_extraction, assessment_year "
                "FROM v_latest_assessment "
                "WHERE UPPER(state) = UPPER('%s') AND UPPER(district) = UPPER('%s') "
                "AND UPPER(block) = UPPER('%s') "
                "ORDER BY assessment_year DESC;",
                state, district ? district : "", block);
    } else if (district && strlen(district) > 0) {
        // Query all blocks in district
        snprintf(query, sizeof(query),
                "SELECT state, district, block, category, annual_recharge, "
                "extractable_resource, annual_extraction, assessment_year "
                "FROM v_latest_assessment "
                "WHERE UPPER(state) = UPPER('%s') AND UPPER(district) = UPPER('%s') "
                "ORDER BY stage_of_extraction DESC;",
                state, district);
    } else {
        // Query all districts in state
        snprintf(query, sizeof(query),
                "SELECT state, district, block, category, annual_recharge, "
                "extractable_resource, annual_extraction, assessment_year "
                "FROM v_latest_assessment "
                "WHERE UPPER(state) = UPPER('%s') "
                "ORDER BY stage_of_extraction DESC;",
                state);
    }
    
    PGresult *res = execute_query(query);
    if (!res) return NULL;
    
    int num_rows = PQntuples(res);
    
    // Create QueryResult
    QueryResult *result = malloc(sizeof(QueryResult));
    if (!result) {
        PQclear(res);
        return NULL;
    }
    
    result->data = malloc(sizeof(GroundwaterData) * num_rows);
    if (!result->data && num_rows > 0) {
        free(result);
        PQclear(res);
        return NULL;
    }
    
    result->count = num_rows;
    snprintf(result->query_type, sizeof(result->query_type), "Location Query");
    result->execution_time_ms = 0.0; // TODO: Add timing
    
    // Convert rows to GroundwaterData
    for (int i = 0; i < num_rows; i++) {
        result->data[i] = pg_row_to_groundwater_data(res, i);
    }
    
    PQclear(res);
    return result;
}

// Query by state
QueryResult* query_by_state(const char* state) {
    return query_by_location(state, NULL, NULL);
}

// Query by category
QueryResult* query_by_category(const char* category) {
    if (!category) return NULL;
    
    char query[1024];
    snprintf(query, sizeof(query),
            "SELECT state, district, block, category, annual_recharge, "
            "extractable_resource, annual_extraction, assessment_year "
            "FROM v_latest_assessment "
            "WHERE UPPER(category) = UPPER('%s') "
            "ORDER BY stage_of_extraction DESC "
            "LIMIT 100;",
            category);
    
    PGresult *res = execute_query(query);
    if (!res) return NULL;
    
    int num_rows = PQntuples(res);
    
    QueryResult *result = malloc(sizeof(QueryResult));
    if (!result) {
        PQclear(res);
        return NULL;
    }
    
    result->data = malloc(sizeof(GroundwaterData) * num_rows);
    result->count = num_rows;
    snprintf(result->query_type, sizeof(result->query_type), "Category Query: %s", category);
    result->execution_time_ms = 0.0;
    
    for (int i = 0; i < num_rows; i++) {
        result->data[i] = pg_row_to_groundwater_data(res, i);
    }
    
    PQclear(res);
    return result;
}

// Query critical areas
QueryResult* query_critical_areas(void) {
    char query[] = 
        "SELECT state, district, block, category, annual_recharge, "
        "extractable_resource, annual_extraction, assessment_year "
        "FROM v_critical_areas "
        "ORDER BY stage_of_extraction DESC "
        "LIMIT 50;";
    
    PGresult *res = execute_query(query);
    if (!res) return NULL;
    
    int num_rows = PQntuples(res);
    
    QueryResult *result = malloc(sizeof(QueryResult));
    if (!result) {
        PQclear(res);
        return NULL;
    }
    
    result->data = malloc(sizeof(GroundwaterData) * num_rows);
    result->count = num_rows;
    snprintf(result->query_type, sizeof(result->query_type), "Critical Areas");
    result->execution_time_ms = 0.0;
    
    for (int i = 0; i < num_rows; i++) {
        result->data[i] = pg_row_to_groundwater_data(res, i);
    }
    
    PQclear(res);
    return result;
}

// Query historical trend
QueryResult* query_historical_trend(const char* state, const char* district, const char* block) {
    if (!state) return NULL;
    
    char query[1024];
    
    if (block && strlen(block) > 0) {
        snprintf(query, sizeof(query),
                "SELECT state, district, block, category, annual_recharge, "
                "extractable_resource, annual_extraction, assessment_year "
                "FROM groundwater_data "
                "WHERE UPPER(state) = UPPER('%s') AND UPPER(district) = UPPER('%s') "
                "AND UPPER(block) = UPPER('%s') "
                "ORDER BY assessment_year DESC "
                "LIMIT 10;",
                state, district ? district : "", block);
    } else {
        snprintf(query, sizeof(query),
                "SELECT state, district, block, category, annual_recharge, "
                "extractable_resource, annual_extraction, assessment_year "
                "FROM groundwater_data "
                "WHERE UPPER(state) = UPPER('%s') "
                "ORDER BY assessment_year DESC, stage_of_extraction DESC "
                "LIMIT 20;",
                state);
    }
    
    PGresult *res = execute_query(query);
    if (!res) return NULL;
    
    int num_rows = PQntuples(res);
    
    QueryResult *result = malloc(sizeof(QueryResult));
    if (!result) {
        PQclear(res);
        return NULL;
    }
    
    result->data = malloc(sizeof(GroundwaterData) * num_rows);
    result->count = num_rows;
    snprintf(result->query_type, sizeof(result->query_type), "Historical Trend");
    result->execution_time_ms = 0.0;
    
    for (int i = 0; i < num_rows; i++) {
        result->data[i] = pg_row_to_groundwater_data(res, i);
    }
    
    PQclear(res);
    return result;
}

// Get database statistics
void print_database_stats(void) {
    if (!db_conn) {
        printf("[DB] No database connection\n");
        return;
    }
    
    // Get total records
    PGresult *res = execute_query("SELECT COUNT(*) FROM groundwater_data;");
    if (res) {
        int total = atoi(PQgetvalue(res, 0, 0));
        printf("[DB] Total records: %d\n", total);
        PQclear(res);
    }
    
    // Get records by category
    res = execute_query(
        "SELECT category, COUNT(*) FROM v_latest_assessment "
        "GROUP BY category ORDER BY COUNT(*) DESC;"
    );
    
    if (res) {
        int num_rows = PQntuples(res);
        printf("[DB] Records by category:\n");
        
        for (int i = 0; i < num_rows; i++) {
            char *category = PQgetvalue(res, i, 0);
            int count = atoi(PQgetvalue(res, i, 1));
            printf("     %s: %d\n", category, count);
        }
        
        PQclear(res);
    }
    
    // Get latest assessment year
    res = execute_query("SELECT MAX(assessment_year) FROM groundwater_data;");
    if (res) {
        int latest_year = atoi(PQgetvalue(res, 0, 0));
        printf("[DB] Latest assessment year: %d\n", latest_year);
        PQclear(res);
    }
}

// Free query result
void free_query_result(QueryResult* result) {
    if (result) {
        if (result->data) {
            free(result->data);
        }
        free(result);
    }
}

// Test database connection and functionality
bool test_database_connection(void) {
    if (!db_conn) {
        printf("[DB TEST] No database connection\n");
        return false;
    }
    
    printf("[DB TEST] Testing database functionality...\n");
    
    // Test 1: Basic query
    QueryResult *result = query_by_state("Punjab");
    if (result && result->count > 0) {
        printf("[DB TEST] ✅ State query successful (%d records)\n", result->count);
        free_query_result(result);
    } else {
        printf("[DB TEST] ❌ State query failed\n");
        return false;
    }
    
    // Test 2: Category query
    result = query_by_category("Over-Exploited");
    if (result) {
        printf("[DB TEST] ✅ Category query successful (%d records)\n", result->count);
        free_query_result(result);
    } else {
        printf("[DB TEST] ❌ Category query failed\n");
        return false;
    }
    
    // Test 3: Critical areas
    result = query_critical_areas();
    if (result) {
        printf("[DB TEST] ✅ Critical areas query successful (%d records)\n", result->count);
        free_query_result(result);
    } else {
        printf("[DB TEST] ❌ Critical areas query failed\n");
        return false;
    }
    
    printf("[DB TEST] ✅ All database tests passed!\n");
    return true;
}