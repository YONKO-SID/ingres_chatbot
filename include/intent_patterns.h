#ifndef INTENT_PATTERNS_H
#define INTENT_PATTERNS_H

#include "chatbot.h"

// Pattern matching structure
typedef struct {
    char* keywords[10];     // Array of keywords to match
    int keyword_count;      // Number of keywords in this pattern
    int priority;          // Higher priority = more specific match
    IntentType intent;     // Which intent this pattern maps to
    bool require_all;      // true = all keywords must match, false = any keyword
} IntentPattern;

// Response template structure
typedef struct {
    IntentType intent;
    char* template_text;
    bool needs_location;   // Does this response need location data?
    bool needs_data;       // Does this response need database data?
} ResponseTemplate;

// Function declarations
extern IntentPattern intent_patterns[];
extern ResponseTemplate response_templates[];
extern int pattern_count;
extern int template_count;

// Pattern matching functions
IntentType match_patterns(const char* user_input);
char* get_response_template(IntentType intent);
void init_patterns(void);

#endif // INTENT_PATTERNS_H