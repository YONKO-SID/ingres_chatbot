#include "database.h"
#include <stdio.h>
#include <stdlib.h>

// Temporary stub implementations for testing
bool db_init(void) {
    printf("[DB] Database initialized (stub mode)\n");
    return true;
}

void db_close(void) {
    printf("[DB] Database closed (stub mode)\n");
}

void free_query_result(QueryResult* result) {
    if (result) {
        if (result->data) free(result->data);
        free(result);
    }
}