/*
 * INGRES ChatBot - Utility Library
 * Combines many of the features of CS50, GLib, and other C libraries
 * has easy-to-use functions for rapid development
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

// ============================================================================
// STRING UTILITIES
// ============================================================================

// String type
typedef char* string;

// String creation and input
string get_string(const char* prompt);
string get_string_nonempty(const char* prompt);
string create_string(const char* initial_value);
string format_string(const char* format, ...);

// String manipulation functions
string string_concat(const string str1, const string str2);
string string_append(string* dest, const string src);
string string_duplicate(const string src);
string string_substring(const string src, int start, int length);
string string_replace(const string src, const string old_str, const string new_str);
string string_trim(const string src);
string string_to_lower(const string src);
string string_to_upper(const string src);

// String analysis
bool string_contains(const string haystack, const string needle);
bool string_starts_with(const string str, const string prefix);
bool string_ends_with(const string str, const string suffix);
bool string_equals(const string str1, const string str2);
bool string_equals_ignore_case(const string str1, const string str2);
int string_length(const string str);
bool string_is_empty(const string str);

// String arrays
typedef struct {
    string* items;
    int count;
    int capacity;
} StringArray;

StringArray* string_array_create(void);
void string_array_add(StringArray* arr, const string item);
string string_array_get(StringArray* arr, int index);
void string_array_free(StringArray* arr);
StringArray* string_split(const string str, const char delimiter);
string string_join(StringArray* arr, const char* separator);

// ============================================================================
// DYNAMIC ARRAYS
// ============================================================================

typedef struct {
    void** items;
    int count;
    int capacity;
    size_t item_size;
} DynamicArray;

DynamicArray* array_create(size_t item_size);
void array_add(DynamicArray* arr, void* item);
void* array_get(DynamicArray* arr, int index);
void array_remove(DynamicArray* arr, int index);
void array_clear(DynamicArray* arr);
void array_free(DynamicArray* arr);
int array_size(DynamicArray* arr);

// ============================================================================
// HASH TABLE / DICTIONARY
// ============================================================================

typedef struct HashNode {
    string key;
    void* value;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode** buckets;
    int bucket_count;
    int size;
} HashTable;

HashTable* hash_create(void);
void hash_set(HashTable* table, const string key, void* value);
void* hash_get(HashTable* table, const string key);
bool hash_contains(HashTable* table, const string key);
void hash_remove(HashTable* table, const string key);
StringArray* hash_keys(HashTable* table);
void hash_free(HashTable* table);

// ============================================================================
// FILE OPERATIONS
// ============================================================================

// File reading/writing
string file_read_all(const string filename);
bool file_write_all(const string filename, const string content);
bool file_append(const string filename, const string content);
bool file_exists(const string filename);
long file_size(const string filename);

// CSV handling
StringArray* csv_read_line(const string csv_line);
StringArray* csv_read_file(const string filename);
bool csv_write_line(const string filename, StringArray* fields);

// JSON-like operations (simple key-value)
HashTable* json_parse_simple(const string json_str);
string json_stringify_simple(HashTable* data);

// ============================================================================
// INPUT/OUTPUT UTILITIES
// ============================================================================

// Safe input functions (CS50-style)
int get_int(const char* prompt);
float get_float(const char* prompt);
double get_double(const char* prompt);
bool get_bool(const char* prompt);
char get_char(const char* prompt);

// Output formatting
void print_colored(const char* color, const char* format, ...);
void print_success(const char* format, ...);
void print_error(const char* format, ...);
void print_warning(const char* format, ...);
void print_info(const char* format, ...);

// Color constants
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_RESET   "\033[0m"

// ============================================================================
// MEMORY MANAGEMENT (Automatic cleanup)
// ============================================================================

// Memory pool for automatic cleanup
typedef struct MemoryPool MemoryPool;

MemoryPool* pool_create(void);
void* pool_malloc(MemoryPool* pool, size_t size);
string pool_string(MemoryPool* pool, const char* str);
void pool_free(MemoryPool* pool);  // Frees everything allocated in pool

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Math utilities
int min_int(int a, int b);
int max_int(int a, int b);
double min_double(double a, double b);
double max_double(double a, double b);
int random_int(int min, int max);
double random_double(double min, double max);

// Time utilities
string get_current_time_string(void);
string get_current_date_string(void);
long get_timestamp_ms(void);

// Validation utilities
bool is_valid_email(const string email);
bool is_valid_number(const string str);
bool is_valid_url(const string url);

// Debug utilities
void debug_print(const char* format, ...);
void debug_print_array(StringArray* arr);
void debug_print_hash(HashTable* table);

// ============================================================================
// CONVENIENCE MACROS
// ============================================================================

// Easy string creation
#define STR(literal) create_string(literal)
#define EMPTY_STR create_string("")

// Easy array operations
#define ARRAY_FOREACH(arr, index, item) \
    for (int index = 0; index < array_size(arr) && (item = array_get(arr, index)); index++)

// Easy string operations
#define STR_EQUALS(s1, s2) string_equals(s1, s2)
#define STR_CONTAINS(haystack, needle) string_contains(haystack, needle)
#define STR_EMPTY(s) string_is_empty(s)

// Easy cleanup
#define SAFE_FREE(ptr) do { if (ptr) { free(ptr); ptr = NULL; } } while(0)

// ============================================================================
// ERROR HANDLING
// ============================================================================

typedef enum {
    UTILS_SUCCESS = 0,
    UTILS_ERROR_NULL_POINTER,
    UTILS_ERROR_OUT_OF_MEMORY,
    UTILS_ERROR_FILE_NOT_FOUND,
    UTILS_ERROR_INVALID_INPUT,
    UTILS_ERROR_BUFFER_OVERFLOW
} UtilsError;

// Get last error
UtilsError utils_get_last_error(void);
string utils_get_error_message(UtilsError error);

#endif // UTILS_H