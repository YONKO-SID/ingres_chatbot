/*
 * INGRES ChatBot - Database Header
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Database configuration
#define MAX_RECORDS 10000
#define MAX_FIELD_LENGTH 100
#define CSV_FILE_PATH "data/groundwater_data.csv"

// Groundwater data structure matching INGRES format
typedef struct {
    char state[MAX_FIELD_LENGTH];
    char district[MAX_FIELD_LENGTH];
    char block[MAX_FIELD_LENGTH];
    float annual_recharge;          // in MCM (Million Cubic Meters)
    float net_extraction;           // in MCM
    float stage_of_extraction;      // in percentage
    char category[MAX_FIELD_LENGTH]; // Safe, Semi-Critical, Critical, Over-Exploited
    int assessment_year;
    float latitude;
    float longitude;
    char tehsil[MAX_FIELD_LENGTH];
} GroundwaterRecord;

// Database structure
typedef struct {
    GroundwaterRecord records[MAX_RECORDS];
    int record_count;
    bool is_loaded;
    char last_error[256];
} GroundwaterDatabase;

// Query result structure
typedef struct {
    GroundwaterRecord* records;
    int count;
    float avg_recharge;
    float avg_extraction;
    char dominant_category[MAX_FIELD_LENGTH];
} QueryResult;

// Database management functions
bool init_database(void);
void cleanup_database(void);
bool load_csv_data(const char* file_path);
bool reload_database(void);

// Query functions
QueryResult* query_by_location(const char* location);
QueryResult* query_by_state(const char* state);
QueryResult* query_by_district(const char* district);
QueryResult* query_by_year(int year);
QueryResult* query_by_year_range(int start_year, int end_year);
QueryResult* query_by_category(const char* category);

// Advanced query functions
QueryResult* query_critical_areas(void);
QueryResult* query_safe_areas(void);
QueryResult* compare_regions(const char* location1, const char* location2);
QueryResult* get_historical_trend(const char* location);

// Data analysis functions
float calculate_average_recharge(QueryResult* result);
float calculate_average_extraction(QueryResult* result);
char* get_dominant_category(QueryResult* result);
int count_by_category(const char* category);

// Utility functions
GroundwaterRecord* find_exact_match(const char* location);
GroundwaterRecord** find_fuzzy_matches(const char* location, int* match_count);
bool is_valid_record(GroundwaterRecord* record);
void print_record(GroundwaterRecord* record);
void print_query_result(QueryResult* result);

// Data validation and cleanup
bool validate_csv_format(const char* file_path);
void clean_field_data(char* field);
bool parse_csv_line(const char* line, GroundwaterRecord* record);

// Memory management
void free_query_result(QueryResult* result);
char* get_database_stats(void);

#endif // DATABASE_H