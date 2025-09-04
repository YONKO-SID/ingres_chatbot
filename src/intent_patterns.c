#include "intent_patterns.h"
#include <string.h>
#include <stdlib.h>

// Define all your intent patterns here
IntentPattern intent_patterns[] = {
    // Greeting patterns (highest priority for exact matches)
    {{"hello", "hi", "namaste", "good morning", "good evening"}, 5, 10, INTENT_GREETING, false},
    
    // Goodbye patterns
    {{"bye", "goodbye", "exit", "quit", "thank you", "thanks"}, 6, 10, INTENT_GOODBYE, false},
    
    // Help patterns
    {{"help", "how to use", "what can you do", "commands", "guide"}, 5, 10, INTENT_HELP, false},
    
    // Status patterns (very specific)
    {{"bot status", "system status", "chatbot status"}, 3, 15, INTENT_STATUS, false},
    
    // Critical areas patterns (high priority)
    {{"which", "areas", "critical"}, 3, 12, INTENT_CRITICAL_AREAS, true},
    {{"critical", "areas"}, 2, 11, INTENT_CRITICAL_AREAS, true},
    {{"show", "critical"}, 2, 11, INTENT_CRITICAL_AREAS, true},
    {{"over-exploited", "over", "exploited"}, 3, 12, INTENT_CRITICAL_AREAS, false},
    {{"dangerous", "areas"}, 2, 10, INTENT_CRITICAL_AREAS, true},
    {{"problem", "areas"}, 2, 10, INTENT_CRITICAL_AREAS, true},
    
    // Safe areas patterns
    {{"safe", "areas"}, 2, 11, INTENT_SAFE_AREAS, true},
    {{"safe", "regions"}, 2, 11, INTENT_SAFE_AREAS, true},
    {{"which", "safe"}, 2, 10, INTENT_SAFE_AREAS, true},
    {{"sustainable", "areas"}, 2, 10, INTENT_SAFE_AREAS, true},
    
    // Water crisis patterns
    {{"water", "crisis"}, 2, 12, INTENT_WATER_CRISIS, true},
    {{"water", "shortage"}, 2, 12, INTENT_WATER_CRISIS, true},
    {{"water", "stress"}, 2, 11, INTENT_WATER_CRISIS, true},
    {{"drought", "areas"}, 2, 11, INTENT_WATER_CRISIS, false},
    {{"water", "scarcity"}, 2, 11, INTENT_WATER_CRISIS, true},
    
    // Rainfall correlation patterns
    {{"rainfall", "groundwater"}, 2, 12, INTENT_RAINFALL_CORRELATION, false},
    {{"monsoon", "impact"}, 2, 11, INTENT_RAINFALL_CORRELATION, false},
    {{"rain", "affect"}, 2, 10, INTENT_RAINFALL_CORRELATION, false},
    {{"precipitation", "groundwater"}, 2, 11, INTENT_RAINFALL_CORRELATION, false},
    {{"climate", "change"}, 2, 10, INTENT_RAINFALL_CORRELATION, false},
    
    // Policy suggestion patterns
    {{"policy", "suggestions"}, 2, 12, INTENT_POLICY_SUGGESTION, false},
    {{"policy", "recommendations"}, 2, 12, INTENT_POLICY_SUGGESTION, false},
    {{"what", "should"}, 2, 9, INTENT_POLICY_SUGGESTION, false},
    {{"how", "improve"}, 2, 9, INTENT_POLICY_SUGGESTION, false},
    {{"solution", "groundwater"}, 2, 10, INTENT_POLICY_SUGGESTION, false},
    {{"measures", "needed"}, 2, 10, INTENT_POLICY_SUGGESTION, false},
    
    // Compare locations patterns
    {{"compare", "punjab", "haryana"}, 3, 15, INTENT_COMPARE_LOCATIONS, false},
    {{"compare"}, 1, 8, INTENT_COMPARE_LOCATIONS, false},
    {{"versus", "vs"}, 2, 9, INTENT_COMPARE_LOCATIONS, false},
    {{"difference", "between"}, 2, 10, INTENT_COMPARE_LOCATIONS, true},
    
    // Historical trend patterns
    {{"trend", "historical"}, 2, 11, INTENT_HISTORICAL_TREND, false},
    {{"past", "data"}, 2, 10, INTENT_HISTORICAL_TREND, true},
    {{"over", "time"}, 2, 9, INTENT_HISTORICAL_TREND, true},
    {{"history", "groundwater"}, 2, 10, INTENT_HISTORICAL_TREND, false},
    {{"change", "over"}, 2, 9, INTENT_HISTORICAL_TREND, false},
    
    // Query category patterns
    {{"show", "over-exploited"}, 2, 11, INTENT_QUERY_CATEGORY, true},
    {{"list", "critical"}, 2, 10, INTENT_QUERY_CATEGORY, true},
    {{"category", "classification"}, 2, 11, INTENT_QUERY_CATEGORY, false},
    
    // Location query patterns (lower priority, more general)
    {{"show", "punjab"}, 2, 8, INTENT_QUERY_LOCATION, false},
    {{"data", "for"}, 2, 7, INTENT_QUERY_LOCATION, false},
    {{"groundwater", "in"}, 2, 8, INTENT_QUERY_LOCATION, false},
    {{"tell", "about"}, 2, 7, INTENT_QUERY_LOCATION, false},
    {{"information", "about"}, 2, 7, INTENT_QUERY_LOCATION, false},
    {{"status", "of"}, 2, 7, INTENT_QUERY_LOCATION, false},
    {{"what", "is"}, 2, 6, INTENT_QUERY_LOCATION, false},
    
    // State name patterns (lowest priority)
    {{"punjab"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"haryana"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"rajasthan"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"gujarat"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"maharashtra"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"karnataka"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"tamil nadu"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"kerala"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"delhi"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"mumbai"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"bangalore"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"chennai"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"kolkata"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"hyderabad"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"amritsar"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"ludhiana"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"jaipur"}, 1, 5, INTENT_QUERY_LOCATION, false},
    {{"ahmedabad"}, 1, 5, INTENT_QUERY_LOCATION, false}
};

// Response templates
ResponseTemplate response_templates[] = {
    {INTENT_GREETING, 
     "Namaste! I'm your INGRES groundwater assistant. I can help you with:\n"
     "* Groundwater data for any location\n"
     "* Critical and over-exploited areas\n"
     "* Historical trends and comparisons\n"
     "* Policy recommendations\n\n"
     "Try asking: 'Show me Punjab groundwater data' or 'Which areas are critical?'", 
     false, false},
     
    {INTENT_GOODBYE,
     "Thank you for using INGRES ChatBot! Stay informed about groundwater resources.",
     false, false},
     
    {INTENT_HELP,
     "INGRES ChatBot Help:\n\n"
     "[LOC] Location Queries: 'Show me [state/district] data'\n"
     "[CRIT] Critical Areas: 'Show critical areas' or 'Over-exploited regions'\n"
     "[COMP] Comparisons: 'Compare Punjab and Haryana'\n"
     "[TREND] Trends: 'Historical trend for Maharashtra'\n"
     "[CRISIS] Water Crisis: 'Areas facing water shortage'\n"
     "[RAIN] Rainfall Impact: 'How does rainfall affect groundwater?'\n"
     "[POLICY] Policy Help: 'Policy suggestions for [location]'\n\n"
     "Example: 'What is the groundwater status of Amritsar?'",
     false, false},
     
    // Add more templates as needed...
};

// Calculate array sizes
int pattern_count = sizeof(intent_patterns) / sizeof(IntentPattern);
int template_count = sizeof(response_templates) / sizeof(ResponseTemplate);

// Pattern matching function
IntentType match_patterns(const char* user_input) {
    if (!user_input) return INTENT_ERROR;
    
    char* lower_input = string_to_lower(user_input);
    if (!lower_input) return INTENT_ERROR;
    
    int best_score = 0;
    IntentType best_intent = INTENT_UNKNOWN;
    
    // Check each pattern
    for (int i = 0; i < pattern_count; i++) {
        IntentPattern* pattern = &intent_patterns[i];
        int matches = 0;
        int total_keywords = pattern->keyword_count;
        
        // Count keyword matches
        for (int j = 0; j < total_keywords; j++) {
            if (strstr(lower_input, pattern->keywords[j])) {
                matches++;
            }
        }
        
        // Calculate score based on pattern requirements
        int score = 0;
        if (pattern->require_all) {
            // All keywords must match
            if (matches == total_keywords) {
                score = pattern->priority;
            }
        } else {
            // Any keyword can match
            if (matches > 0) {
                score = pattern->priority * matches / total_keywords;
            }
        }
        
        // Update best match
        if (score > best_score) {
            best_score = score;
            best_intent = pattern->intent;
        }
    }
    
    free(lower_input);
    return best_intent;
}

// Get response template for an intent
char* get_response_template(IntentType intent) {
    for (int i = 0; i < template_count; i++) {
        if (response_templates[i].intent == intent) {
            return strdup(response_templates[i].template_text);
        }
    }
    return strdup("[HELP] I didn't quite understand that. Type 'help' for more options.");
}

// Initialize patterns (if needed for dynamic loading)
void init_patterns(void) {
    // This function can be used to load patterns from a file in the future
    // For now, patterns are statically defined
}