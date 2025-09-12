#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Include the sample data
#include "../data/sample_groundwater.c"

// Global database structure
static struct {
    GroundwaterData* records;
    int record_count;
    bool initialized;
} db = {0};

// Initialize database with sample data
bool db_init(void) {
    if (db.initialized) {
        return true;
    }
    
    printf("Initializing database with sample groundwater data...\n");
    
    // Use the sample data from the included file
    db.records = sample_data;
    db.record_count = sizeof(sample_data) / sizeof(sample_data[0]);
    db.initialized = true;
    
    printf("Loaded %d groundwater assessment records\n", db.record_count);
    return true;
}

// Close database (no-op for sample data)
void db_close(void) {
    printf("Closing database...\n");
    db.initialized = false;
    db.records = NULL;
    db.record_count = 0;
}

// Query by location
QueryResult* query_by_location(const char* state, const char* district, const char* block) {
    if (!db.initialized || !state || !district) {
        return NULL;
    }
    
    clock_t start = clock();
    
    // Count matching records
    int count = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].state, state) == 0 &&
            strcasecmp(db.records[i].district, district) == 0) {
            if (!block || strcasecmp(db.records[i].block, block) == 0) {
                count++;
            }
        }
    }
    
    if (count == 0) {
        return NULL;
    }
    
    // Allocate result
    QueryResult* result = malloc(sizeof(QueryResult));
    result->data = malloc(count * sizeof(GroundwaterData));
    result->count = count;
    strcpy(result->query_type, "location_query");
    
    // Copy matching records
    int index = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].state, state) == 0 &&
            strcasecmp(db.records[i].district, district) == 0) {
            if (!block || strcasecmp(db.records[i].block, block) == 0) {
                result->data[index++] = db.records[i];
            }
        }
    }
    
    result->execution_time_ms = ((double)(clock() - start)) * 1000 / CLOCKS_PER_SEC;
    return result;
}

// Query by state
QueryResult* query_by_state(const char* state) {
    if (!db.initialized || !state) {
        return NULL;
    }
    
    clock_t start = clock();
    
    int count = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].state, state) == 0) {
            count++;
        }
    }
    
    if (count == 0) {
        return NULL;
    }
    
    QueryResult* result = malloc(sizeof(QueryResult));
    result->data = malloc(count * sizeof(GroundwaterData));
    result->count = count;
    strcpy(result->query_type, "state_query");
    
    int index = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].state, state) == 0) {
            result->data[index++] = db.records[i];
        }
    }
    
    result->execution_time_ms = ((double)(clock() - start)) * 1000 / CLOCKS_PER_SEC;
    return result;
}

// Query by category
QueryResult* query_by_category(const char* category) {
    if (!db.initialized || !category) {
        return NULL;
    }
    
    clock_t start = clock();
    
    int count = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].category, category) == 0) {
            count++;
        }
    }
    
    if (count == 0) {
        return NULL;
    }
    
    QueryResult* result = malloc(sizeof(QueryResult));
    result->data = malloc(count * sizeof(GroundwaterData));
    result->count = count;
    strcpy(result->query_type, "category_query");
    
    int index = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].category, category) == 0) {
            result->data[index++] = db.records[i];
        }
    }
    
    result->execution_time_ms = ((double)(clock() - start)) * 1000 / CLOCKS_PER_SEC;
    return result;
}

// Query critical areas
QueryResult* query_critical_areas(void) {
    if (!db.initialized) {
        return NULL;
    }
    
    clock_t start = clock();
    
    int count = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].category, "Critical") == 0 ||
            strcasecmp(db.records[i].category, "Over-Exploited") == 0) {
            count++;
        }
    }
    
    if (count == 0) {
        return NULL;
    }
    
    QueryResult* result = malloc(sizeof(QueryResult));
    result->data = malloc(count * sizeof(GroundwaterData));
    result->count = count;
    strcpy(result->query_type, "critical_areas");
    
    int index = 0;
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].category, "Critical") == 0 ||
            strcasecmp(db.records[i].category, "Over-Exploited") == 0) {
            result->data[index++] = db.records[i];
        }
    }
    
    result->execution_time_ms = ((double)(clock() - start)) * 1000 / CLOCKS_PER_SEC;
    return result;
}

// Query historical trend (simplified for sample data)
QueryResult* query_historical_trend(const char* state, const char* district, const char* block) {
    return query_by_location(state, district, block);
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

// Print database statistics
void print_database_stats(void) {
    if (!db.initialized) {
        printf("Database not initialized\n");
        return;
    }
    
    int safe = 0, semi_critical = 0, critical = 0, over_exploited = 0;
    
    for (int i = 0; i < db.record_count; i++) {
        if (strcasecmp(db.records[i].category, "Safe") == 0) {
            safe++;
        } else if (strcasecmp(db.records[i].category, "Semi-Critical") == 0) {
            semi_critical++;
        } else if (strcasecmp(db.records[i].category, "Critical") == 0) {
            critical++;
        } else if (strcasecmp(db.records[i].category, "Over-Exploited") == 0) {
            over_exploited++;
        }
    }
    
    printf("\n=== Database Statistics ===\n");
    printf("Total records: %d\n", db.record_count);
    printf("Safe areas: %d\n", safe);
    printf("Semi-Critical areas: %d\n", semi_critical);
    printf("Critical areas: %d\n", critical);
    printf("Over-Exploited areas: %d\n", over_exploited);
    printf("========================\n\n");
}