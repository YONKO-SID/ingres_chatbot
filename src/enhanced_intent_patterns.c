/**
 * Enhanced Intent Patterns - Fixed Version
 * 
 * This file contains comprehensive fixes for the intent classification system:
 * 1. Reduced confidence thresholds for better matching
 * 2. Enhanced keyword variations for natural language support
 * 3. Improved scoring algorithm with better normalization
 * 4. Added comprehensive fuzzy matching
 * 5. Enhanced location and context detection
 */

#include "chatbot.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

// Enhanced pattern structure with better matching capabilities
typedef struct {
    char* keywords[20];         // Increased keyword capacity for better matching
    char* synonyms[25];         // More synonyms for natural language variations
    char* context_keywords[15]; // Context-dependent keywords for better accuracy
    int keyword_count;
    int synonym_count;
    int context_count;
    int priority;
    IntentType intent;
    bool require_all;
    bool context_dependent;
    float min_confidence;       // Optimized confidence thresholds
    char* example_queries[8];   // More examples for testing
    int example_count;
} EnhancedIntentPattern;

// Indian state and city names for location extraction
const char* indian_states[] = {
    "andhra pradesh", "arunachal pradesh", "assam", "bihar", "chhattisgarh", "goa", "gujarat",
    "haryana", "himachal pradesh", "jharkhand", "karnataka", "kerala", "madhya pradesh",
    "maharashtra", "manipur", "meghalaya", "mizoram", "nagaland", "odisha", "punjab",
    "rajasthan", "sikkim", "tamil nadu", "telangana", "tripura", "uttar pradesh",
    "uttarakhand", "west bengal", "delhi", "jammu and kashmir", "ladakh"
};

const char* major_cities[] = {
    "mumbai", "delhi", "bangalore", "hyderabad", "ahmedabad", "chennai", "kolkata",
    "surat", "pune", "jaipur", "lucknow", "kanpur", "nagpur", "indore", "thane",
    "bhopal", "visakhapatnam", "pimpri", "patna", "vadodara", "ghaziabad", "ludhiana",
    "agra", "nashik", "faridabad", "meerut", "rajkot", "kalyan", "vasai", "varanasi",
    "srinagar", "aurangabad", "dhanbad", "amritsar", "navi mumbai", "allahabad",
    "ranchi", "howrah", "coimbatore", "jabalpur", "gwalior", "vijayawada", "jodhpur",
    "madurai", "raipur", "kota", "chandigarh", "guwahati", "solapur", "hubli"
};

int state_count = sizeof(indian_states) / sizeof(char*);
int city_count = sizeof(major_cities) / sizeof(char*);

// Levenshtein distance for fuzzy matching
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1), len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];
    
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = fmin(fmin(matrix[i-1][j] + 1, matrix[i][j-1] + 1), 
                               matrix[i-1][j-1] + cost);
        }
    }
    
    return matrix[len1][len2];
}

float calculate_similarity(const char* str1, const char* str2) {
    if (!str1 || !str2) return 0.0;
    
    int len1 = strlen(str1), len2 = strlen(str2);
    if (len1 == 0 && len2 == 0) return 1.0;
    if (len1 == 0 || len2 == 0) return 0.0;
    
    int distance = levenshtein_distance(str1, str2);
    int max_len = (len1 > len2) ? len1 : len2;
    
    return 1.0 - ((float)distance / max_len);
}

// Fixed enhanced patterns with optimized thresholds and comprehensive keywords
EnhancedIntentPattern enhanced_patterns[] = {
    // === GREETING PATTERNS - Reduced threshold for better greeting detection ===
    {
        .keywords = {"hello", "hi", "namaste", "good morning", "good evening", "hey", "greetings", 
                   "hi there", "hello there", "good day", "salaam", "vanakkam", "namaskar"},
        .synonyms = {"hola", "bonjour", "salaam", "vanakkam", "sat sri akal", "adaab", 
                     "yo", "wassup", "howdy", "g'day", "aloha"},
        .keyword_count = 13, .synonym_count = 11, .priority = 15, .intent = INTENT_GREETING,
        .require_all = false, .context_dependent = false, .min_confidence = 0.4,  // Reduced from 0.8
        .example_queries = {"Hello", "Hi there", "Good morning", "Namaste", "Hey chatbot", 
                          "Hi!", "Hello there", "Good day"},
        .example_count = 8
    },
    
    // === LOCATION QUERY PATTERNS - More forgiving for location-based queries ===
    {
        .keywords = {"show", "data", "for", "groundwater", "in", "tell", "about", "give", "display"},
        .synonyms = {"display", "information", "details", "stats", "statistics", "info", "tell me",
                     "provide", "present", "show me"},
        .context_keywords = {"punjab", "haryana", "gujarat", "maharashtra", "rajasthan", "delhi",
                           "uttar pradesh", "madhya pradesh", "karnataka", "tamil nadu"},
        .keyword_count = 9, .synonym_count = 10, .context_count = 10, .priority = 12,
        .intent = INTENT_QUERY_LOCATION, .require_all = false, .context_dependent = true,
        .min_confidence = 0.35,  // Reduced from 0.7
        .example_queries = {"Show me Punjab data", "Groundwater in Maharashtra", "Data for Gujarat",
                          "Tell me about Punjab", "Give me Haryana info"},
        .example_count = 5
    },
    
    // === DISTRICT-LEVEL QUERIES - Better district detection ===
    {
        .keywords = {"district", "data", "show", "me", "block", "tehsil", "area"},
        .synonyms = {"information", "details", "stats", "display", "region", "locality",
                     "zone", "sector", "neighborhood"},
        .context_keywords = {"amritsar", "ludhiana", "pune", "ahmedabad", "jaipur", "mumbai",
                           "delhi", "bangalore", "hyderabad", "chennai"},
        .keyword_count = 7, .synonym_count = 9, .context_count = 10, .priority = 13,
        .intent = INTENT_QUERY_DISTRICT, .require_all = false, .context_dependent = true,
        .min_confidence = 0.4,  // Reduced from 0.75
        .example_queries = {"Show me Amritsar district", "Pune district data", "Ludhiana information",
                          "Tell me about Mumbai", "Ahmedabad area details"},
        .example_count = 5
    },
    
    // === CRITICAL AREAS - More sensitive crisis detection ===
    {
        .keywords = {"critical", "areas", "which", "show", "danger", "crisis", "alert"},
        .synonyms = {"dangerous", "problematic", "concerning", "alarming", "severe", "critical",
                     "urgent", "emergency", "red zone", "warning"},
        .context_keywords = {"over-exploited", "crisis", "emergency", "urgent", "danger", 
                           "threat", "alarming", "severe"},
        .keyword_count = 7, .synonym_count = 10, .context_count = 8, .priority = 14,
        .intent = INTENT_CRITICAL_AREAS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.45,  // Reduced from 0.8
        .example_queries = {"Which areas are critical?", "Show critical regions", "Dangerous areas",
                          "Critical zones", "Emergency areas"},
        .example_count = 5
    },
    
    // === OVER-EXPLOITED AREAS - Better depletion detection ===
    {
        .keywords = {"over-exploited", "over", "exploited", "areas", "depleted", "exhausted"},
        .synonyms = {"overused", "depleted", "exhausted", "mining", "overdraft", "overpump",
                     "unsustainable", "overwithdrawal", "depletion"},
        .keyword_count = 6, .synonym_count = 9, .priority = 15,
        .intent = INTENT_OVER_EXPLOITED_AREAS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.5,  // Reduced from 0.85
        .example_queries = {"Over-exploited areas", "Show overused regions", "Depleted zones",
                          "Exhausted groundwater", "Overdraft areas"},
        .example_count = 5
    },
    
    // === COMPARISON PATTERNS - More flexible comparison detection ===
    {
        .keywords = {"compare", "vs", "versus", "difference", "between", "better", "worse"},
        .synonyms = {"contrast", "differentiate", "analyze", "examine", "compare with",
                     "how does", "versus", "compared to", "relatively"},
        .context_keywords = {"punjab", "haryana", "gujarat", "states", "regions", "areas",
                           "districts", "blocks", "tehsils"},
        .keyword_count = 7, .synonym_count = 9, .context_count = 9, .priority = 13,
        .intent = INTENT_COMPARE_LOCATIONS, .require_all = false, .context_dependent = true,
        .min_confidence = 0.4,  // Reduced from 0.75
        .example_queries = {"Compare Punjab vs Haryana", "Difference between Gujarat and Rajasthan",
                          "Punjab vs Haryana", "How does Punjab compare"},
        .example_count = 4
    },
    
    // === TREND ANALYSIS - Better historical analysis detection ===
    {
        .keywords = {"trend", "historical", "over", "time", "change", "past", "years"},
        .synonyms = {"pattern", "evolution", "development", "progression", "trajectory",
                     "history", "timeline", "changes", "variations", "fluctuations"},
        .context_keywords = {"years", "decade", "annual", "monthly", "seasonal", "historical",
                           "past", "previous", "earlier"},
        .keyword_count = 7, .synonym_count = 10, .context_count = 9, .priority = 12,
        .intent = INTENT_HISTORICAL_TREND, .require_all = false, .context_dependent = false,
        .min_confidence = 0.35,  // Reduced from 0.7
        .example_queries = {"Historical trend for Punjab", "Groundwater change over time",
                          "Past 10 years trend", "Annual changes"},
        .example_count = 4
    },
    
    // === POLICY SUGGESTIONS - Better policy detection ===
    {
        .keywords = {"policy", "suggestions", "recommendations", "what", "should", "how to"},
        .synonyms = {"advice", "guidance", "measures", "solutions", "strategies", "recommend",
                     "suggest", "propose", "what can be done", "how should"},
        .context_keywords = {"government", "conservation", "management", "regulation", "policy",
                           "measures", "steps", "actions"},
        .keyword_count = 7, .synonym_count = 10, .context_count = 8, .priority = 11,
        .intent = INTENT_POLICY_SUGGESTION, .require_all = false, .context_dependent = false,
        .min_confidence = 0.35,  // Reduced from 0.7
        .example_queries = {"Policy suggestions for Punjab", "What should government do?",
                          "Conservation measures", "How to improve"},
        .example_count = 4
    },
    
    // === CONSERVATION METHODS - Better conservation detection ===
    {
        .keywords = {"conservation", "methods", "techniques", "how", "to", "save", "preserve"},
        .synonyms = {"preservation", "protection", "sustainability", "efficiency", "save",
                     "conserve", "methods", "ways", "techniques", "approaches"},
        .keyword_count = 7, .synonym_count = 10, .priority = 12,
        .intent = INTENT_CONSERVATION_METHODS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.4,  // Reduced from 0.75
        .example_queries = {"Water conservation methods", "How to save groundwater", "Conservation techniques",
                          "Ways to preserve water", "Saving methods"},
        .example_count = 5
    },
    
    // === RAINFALL CORRELATION - Better weather impact detection ===
    {
        .keywords = {"rainfall", "monsoon", "affect", "impact", "groundwater", "weather", "climate"},
        .synonyms = {"precipitation", "rain", "weather", "climate", "influence", "effect",
                     "monsoon", "seasonal", "rainwater", "meteorological"},
        .keyword_count = 7, .synonym_count = 10, .priority = 11,
        .intent = INTENT_RAINFALL_CORRELATION, .require_all = false, .context_dependent = false,
        .min_confidence = 0.35,  // Reduced from 0.7
        .example_queries = {"How does rainfall affect groundwater?", "Monsoon impact on water table",
                          "Rainfall influence", "Weather effects"},
        .example_count = 4
    },
    
    // === AGRICULTURE IMPACT - Better farming impact detection ===
    {
        .keywords = {"agriculture", "farming", "crops", "irrigation", "impact", "farm", "cultivation"},
        .synonyms = {"cultivation", "agricultural", "farm", "crop", "harvest", "farming practices",
                     "agronomy", "tillage", "planting", "agricultural impact"},
        .context_keywords = {"rice", "wheat", "sugarcane", "cotton", "water-intensive", "crops",
                           "farming", "irrigation", "agriculture"},
        .keyword_count = 7, .synonym_count = 11, .context_count = 9, .priority = 10,
        .intent = INTENT_AGRICULTURE_IMPACT, .require_all = false, .context_dependent = false,
        .min_confidence = 0.3,  // Reduced from 0.65
        .example_queries = {"Agriculture impact on groundwater", "Farming effects on water table",
                          "Crop irrigation impact", "Agricultural influence"},
        .example_count = 4
    },
    
    // === TECHNICAL EXPLANATION - Better technical query detection ===
    {
        .keywords = {"explain", "what", "is", "stage", "extraction", "mean", "define", "clarify"},
        .synonyms = {"define", "clarify", "describe", "elaborate", "meaning", "explanation",
                     "interpretation", "definition", "description", "clarification"},
        .context_keywords = {"technical", "calculation", "methodology", "formula", "explanation",
                           "meaning", "definition"},
        .keyword_count = 8, .synonym_count = 10, .context_count = 7, .priority = 11,
        .intent = INTENT_TECHNICAL_EXPLANATION, .require_all = false, .context_dependent = false,
        .min_confidence = 0.3,  // Reduced from 0.7
        .example_queries = {"What is stage of extraction?", "Explain groundwater categories",
                          "Define water table", "Technical explanation"},
        .example_count = 4
    },
    
    // === CRISIS & EMERGENCY PATTERNS - Better crisis detection ===
    {
        .keywords = {"water", "crisis", "emergency", "shortage", "scarcity", "urgent", "help"},
        .synonyms = {"drought", "deficit", "lack", "depletion", "stress", "emergency",
                     "crisis", "shortage", "problem", "issue"},
        .context_keywords = {"urgent", "immediate", "critical", "severe", "emergency",
                           "crisis", "shortage", "help"},
        .keyword_count = 7, .synonym_count = 10, .context_count = 8, .priority = 13,
        .intent = INTENT_WATER_CRISIS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.35,  // Reduced from 0.75
        .example_queries = {"Water crisis areas", "Emergency water shortage", "Drought affected regions",
                          "Urgent help needed", "Water emergency"},
        .example_count = 5
    },
    
    // === ECONOMIC & SOCIAL IMPACT - Better impact analysis ===
    {
        .keywords = {"economic", "cost", "impact", "financial", "social", "money", "expensive"},
        .synonyms = {"monetary", "expense", "budget", "society", "community", "cost",
                     "financial", "economic", "money", "price"},
        .keyword_count = 7, .synonym_count = 10, .priority = 9,
        .intent = INTENT_ECONOMIC_IMPACT, .require_all = false, .context_dependent = false,
        .min_confidence = 0.3,  // Reduced from 0.65
        .example_queries = {"Economic impact of water crisis", "Cost of groundwater depletion",
                          "Financial implications", "Social impact"},
        .example_count = 4
    },
    
    // === FOLLOW-UP & CONTEXT PATTERNS - Better conversation flow ===
    {
        .keywords = {"tell", "me", "more", "about", "that", "elaborate", "explain", "details"},
        .synonyms = {"explain", "details", "further", "additional", "expand", "clarify",
                     "elaborate", "tell me more", "continue", "proceed"},
        .keyword_count = 8, .synonym_count = 11, .priority = 8,
        .intent = INTENT_FOLLOW_UP_QUESTION, .require_all = false, .context_dependent = true,
        .min_confidence = 0.25,  // Reduced from 0.6
        .example_queries = {"Tell me more about that", "Can you elaborate?", "More details please",
                          "Explain further", "Continue"},
        .example_count = 5
    },
    
    // === HELP & GUIDANCE - Better help detection ===
    {
        .keywords = {"help", "guide", "assistance", "support", "how", "what", "can", "you", "do"},
        .synonyms = {"assist", "aid", "support", "guide", "help me", "what can you do",
                     "capabilities", "features", "functions"},
        .keyword_count = 9, .synonym_count = 10, .priority = 14,
        .intent = INTENT_HELP, .require_all = false, .context_dependent = false,
        .min_confidence = 0.3,  // Reduced from 0.7
        .example_queries = {"Help me", "What can you do?", "Guide me", "I need assistance",
                          "Show me capabilities", "Help"},
        .example_count = 6
    }
};

// Calculate the number of enhanced patterns
const int ENHANCED_PATTERN_COUNT = sizeof(enhanced_patterns) / sizeof(EnhancedIntentPattern);

// Enhanced fuzzy matching function
static bool fuzzy_match(const char* str1, const char* str2, float threshold) {
    if (!str1 || !str2) return false;
    
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    if (len1 == 0 || len2 == 0) return false;
    
    // Direct match
    if (strcasecmp(str1, str2) == 0) return true;
    
    // Substring match
    if (len1 <= len2 && strncasecmp(str1, str2, len1) == 0) return true;
    if (len2 <= len1 && strncasecmp(str2, str1, len2) == 0) return true;
    
    // Levenshtein distance for fuzzy matching
    int distance = 0;
    int max_len = (len1 > len2) ? len1 : len2;
    
    // Simple distance calculation for performance
    int i, j;
    int matrix[len1 + 1][len2 + 1];
    
    for (i = 0; i <= len1; i++) matrix[i][0] = i;
    for (j = 0; j <= len2; j++) matrix[0][j] = j;
    
    for (i = 1; i <= len1; i++) {
        for (j = 1; j <= len2; j++) {
            int cost = (tolower(str1[i-1]) == tolower(str2[j-1])) ? 0 : 1;
            matrix[i][j] = (matrix[i-1][j] + 1 < matrix[i][j-1] + 1) ? 
                          (matrix[i-1][j] + 1 < matrix[i-1][j-1] + cost) ? 
                          matrix[i-1][j] + 1 : matrix[i-1][j-1] + cost : 
                          (matrix[i][j-1] + 1 < matrix[i-1][j-1] + cost) ? 
                          matrix[i][j-1] + 1 : matrix[i-1][j-1] + cost;
        }
    }
    
    distance = matrix[len1][len2];
    float similarity = 1.0f - ((float)distance / (float)max_len);
    
    return similarity >= threshold;
}

// Enhanced intent classification with better scoring
IntentType classify_intent_advanced(const char* user_input, ConversationContext* context, float* confidence) {
    if (!user_input || !confidence) {
        *confidence = 0.0f;
        return INTENT_UNKNOWN;
    }
    
    printf("[DEBUG] classify_intent_advanced called with: '%s'\n", user_input);
    
    char* lower_input = strdup(user_input);
    for (int i = 0; lower_input[i]; i++) {
        lower_input[i] = tolower(lower_input[i]);
    }
    printf("[DEBUG] Lowercase input: '%s'\n", lower_input);
    
    printf("[DEBUG] Number of enhanced patterns: %d\n", ENHANCED_PATTERN_COUNT);
    
    IntentType best_intent = INTENT_UNKNOWN;
    float best_score = 0.0f;
    
    for (int i = 0; i < ENHANCED_PATTERN_COUNT; i++) {
        EnhancedIntentPattern* pattern = &enhanced_patterns[i];
        float score = 0.0f;
        float total_possible = 0.0f;
        
        // Check keywords
        for (int j = 0; j < pattern->keyword_count; j++) {
            if (fuzzy_match(pattern->keywords[j], lower_input, 0.8f)) {
                score += 2.0f; // Keywords get higher weight
            }
            total_possible += 2.0f;
        }
        
        // Check synonyms
        for (int j = 0; j < pattern->synonym_count; j++) {
            if (fuzzy_match(pattern->synonyms[j], lower_input, 0.8f)) {
                score += 1.5f; // Synonyms get moderate weight
            }
            total_possible += 1.5f;
        }
        
        // Check context keywords if context-dependent
        if (pattern->context_dependent && context && context->last_location) {
            for (int j = 0; j < pattern->context_count; j++) {
                if (fuzzy_match(pattern->context_keywords[j], context->last_location, 0.9f)) {
                    score += 1.0f; // Context match bonus
                }
                total_possible += 1.0f;
            }
        }
        
        // Normalize score
        float normalized_score = total_possible > 0 ? score / total_possible : 0.0f;
        
        // Apply priority boost
        normalized_score *= (1.0f + (pattern->priority / 20.0f));
        
        // Check if meets minimum confidence
        if (normalized_score >= pattern->min_confidence && normalized_score > best_score) {
            best_score = normalized_score;
            best_intent = pattern->intent;
        }
    }
    
    free(lower_input);
    
    *confidence = best_score;
    printf("[DEBUG] Best intent: %d, Confidence: %.2f\n", best_intent, best_score);
    
    return best_intent;
}

// Extract locations from user input
int extract_locations(const char* user_input, char** state, char** district, char** block) {
    if (!user_input) return 0;
    
    char* lower_input = string_to_lower(user_input);
    int locations_found = 0;
    
    *state = NULL;
    *district = NULL;
    *block = NULL;
    
    // Check for state names
    for (int i = 0; i < state_count; i++) {
        if (strstr(lower_input, indian_states[i])) {
            *state = strdup(indian_states[i]);
            locations_found++;
            break;
        }
    }
    
    // Check for city names (could be districts)
    for (int i = 0; i < city_count; i++) {
        if (strstr(lower_input, major_cities[i])) {
            *district = strdup(major_cities[i]);
            locations_found++;
            break;
        }
    }
    
    // Fuzzy matching for misspelled locations
    if (locations_found == 0) {
        char* words[20];
        int word_count = 0;
        char* input_copy = strdup(lower_input);
        char* token = strtok(input_copy, " ");
        
        while (token && word_count < 20) {
            words[word_count++] = token;
            token = strtok(NULL, " ");
        }
        
        // Check each word against state names
        for (int i = 0; i < word_count; i++) {
            for (int j = 0; j < state_count; j++) {
                float similarity = calculate_similarity(words[i], indian_states[j]);
                if (similarity > 0.8) {  // 80% similarity for location names
                    *state = strdup(indian_states[j]);
                    locations_found++;
                    break;
                }
            }
            if (*state) break;
        }
        
        free(input_copy);
    }
    
    free(lower_input);
    return locations_found;
}

// Initialize conversation context
ConversationContext* init_conversation_context(void) {
    ConversationContext* context = malloc(sizeof(ConversationContext));
    if (!context) return NULL;
    
    context->last_location = NULL;
    context->last_state = NULL;
    context->last_district = NULL;
    context->last_intent = INTENT_UNKNOWN;
    context->history_count = 0;
    context->awaiting_clarification = false;
    context->pending_question = NULL;
    context->session_start = time(NULL);
    context->query_count = 0;
    
    for (int i = 0; i < 10; i++) {
        context->conversation_history[i] = NULL;
    }
    
    return context;
}

// Update conversation context
void update_conversation_context(ConversationContext* context, const char* user_input, 
                               IntentType intent, const char* location) {
    if (!context) return;
    
    // Update last intent
    context->last_intent = intent;
    context->query_count++;
    
    // Update location context
    if (location) {
        if (context->last_location) free(context->last_location);
        context->last_location = strdup(location);
    }
    
    // Add to conversation history
    if (context->history_count < 10) {
        context->conversation_history[context->history_count] = strdup(user_input);
        context->history_count++;
    } else {
        // Shift history and add new entry
        free(context->conversation_history[0]);
        for (int i = 0; i < 9; i++) {
            context->conversation_history[i] = context->conversation_history[i + 1];
        }
        context->conversation_history[9] = strdup(user_input);
    }
}

// Free conversation context
void free_conversation_context(ConversationContext* context) {
    if (!context) return;
    
    if (context->last_location) free(context->last_location);
    if (context->last_state) free(context->last_state);
    if (context->last_district) free(context->last_district);
    if (context->pending_question) free(context->pending_question);
    
    for (int i = 0; i < context->history_count; i++) {
        if (context->conversation_history[i]) {
            free(context->conversation_history[i]);
        }
    }
    
    free(context);
}

// Get pattern count for external access
int get_enhanced_pattern_count(void) {
    return ENHANCED_PATTERN_COUNT;
}

// Get pattern info for debugging
const EnhancedIntentPattern* get_enhanced_pattern(int index) {
    if (index >= 0 && index < ENHANCED_PATTERN_COUNT) {
        return &enhanced_patterns[index];
    }
    return NULL;
}