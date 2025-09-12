#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void print_error(const char* message) {
    printf("ERROR: %s\n", message);
}

char* string_to_lower(const char* str) {
    if (!str) return NULL;
    
    int len = strlen(str);
    char* result = malloc(len + 1);
    if (!result) return NULL;
    
    for (int i = 0; i < len; i++) {
        result[i] = tolower(str[i]);
    }
    result[len] = '\0';
    
    return result;
}