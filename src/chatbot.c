#include "chatbot.h"
#include "database.h"
#include "utils.h"
#include "intent_patterns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Global chatbot state
static struct {
    bool initialized;
    ConversationContext context;
} chatbot = {0};

// Initialize chatbot
bool chatbot_init(void) {
    if (chatbot.initialized) {
        return true;
    }
    
    printf("Initializing INGRES ChatBot...\n");
    
    // Initialize database
    if (!db_init()) {
        printf("Failed to initialize database\n");
        return false;
    }
    
    // Initialize conversation context
    memset(&chatbot.context, 0, sizeof(ConversationContext));
    chatbot.context.session_start = time(NULL);
    chatbot.initialized = true;
    
    printf("ChatBot initialized successfully\n");
    return true;
}

// Cleanup chatbot
void chatbot_cleanup(void) {
    if (!chatbot.initialized) {
        return;
    }
    
    // Free context memory
    if (chatbot.context.last_location) free(chatbot.context.last_location);
    if (chatbot.context.last_state) free(chatbot.context.last_state);
    if (chatbot.context.last_district) free(chatbot.context.last_district);
    if (chatbot.context.pending_question) free(chatbot.context.pending_question);
    
    for (int i = 0; i < chatbot.context.history_count; i++) {
        if (chatbot.context.conversation_history[i]) {
            free(chatbot.context.conversation_history[i]);
        }
    }
    
    db_close();
    chatbot.initialized = false;
    printf("ChatBot cleanup completed\n");
}

// Enhanced intent classification
IntentType classify_intent(const char* user_input) {
    if (!user_input) return INTENT_UNKNOWN;
    
    char* lower_input = strdup(user_input);
    for (char* p = lower_input; *p; ++p) *p = tolower(*p);
    
    // Remove leading/trailing whitespace
    char* trimmed = lower_input;
    while (*trimmed == ' ' || *trimmed == '\t' || *trimmed == '\n') trimmed++;
    
    // Enhanced keyword matching with better patterns
    
    // Critical areas - more comprehensive matching
    if (strstr(trimmed, "critical") || strstr(trimmed, "danger") || 
        strstr(trimmed, "problem") || strstr(trimmed, "over-exploit") ||
        strstr(trimmed, "over exploited") || (strstr(trimmed, "areas") && 
        (strstr(trimmed, "critical") || strstr(trimmed, "danger")))) {
        free(lower_input);
        return INTENT_CRITICAL_AREAS;
    }
    
    // Safe areas
    if (strstr(trimmed, "safe") || strstr(trimmed, "good") || 
        strstr(trimmed, "sustainable") || strstr(trimmed, "secure")) {
        free(lower_input);
        return INTENT_SAFE_AREAS;
    }
    
    // Statistics
    if (strstr(trimmed, "statistics") || strstr(trimmed, "stats") || 
        strstr(trimmed, "summary") || strstr(trimmed, "overview") ||
        strstr(trimmed, "data count") || strstr(trimmed, "total")) {
        free(lower_input);
        return INTENT_SUMMARY_STATISTICS;
    }
    
    // Location queries - more specific patterns
    if (strstr(trimmed, "show me") || strstr(trimmed, "tell me") || 
        strstr(trimmed, "what is") || strstr(trimmed, "data for") ||
        strstr(trimmed, "groundwater in") || strstr(trimmed, "status of") ||
        strstr(trimmed, "information about") || strstr(trimmed, "details about")) {
        free(lower_input);
        return INTENT_QUERY_LOCATION;
    }
    
    // Greetings
    if (strstr(trimmed, "hello") || strstr(trimmed, "hi") || 
        strstr(trimmed, "hey") || strstr(trimmed, "namaste") ||
        strstr(trimmed, "good morning") || strstr(trimmed, "good evening")) {
        free(lower_input);
        return INTENT_GREETING;
    }
    
    // Help
    if (strstr(trimmed, "help") || strstr(trimmed, "?") || 
        strstr(trimmed, "how to") || strstr(trimmed, "what can") ||
        strstr(trimmed, "guide")) {
        free(lower_input);
        return INTENT_HELP;
    }
    
    // Goodbye
    if (strstr(trimmed, "bye") || strstr(trimmed, "goodbye") || 
        strstr(trimmed, "exit") || strstr(trimmed, "quit") ||
        strstr(trimmed, "thanks") || strstr(trimmed, "thank you")) {
        free(lower_input);
        return INTENT_GOODBYE;
    }
    
    // Check for state names directly
    const char* states[] = {"punjab", "haryana", "gujarat", "maharashtra", "rajasthan", 
                           "uttar pradesh", "tamil nadu", "karnataka", "telangana", 
                           "andhra pradesh", "delhi", "mumbai", "bangalore", "chennai"};
    
    for (int i = 0; i < sizeof(states)/sizeof(states[0]); i++) {
        if (strstr(trimmed, states[i])) {
            free(lower_input);
            return INTENT_QUERY_LOCATION;
        }
    }
    
    free(lower_input);
    return INTENT_UNKNOWN;
}

// Extract location from user input
static void extract_location(const char* user_input, char* state, char* district, char* block) {
    if (!user_input) return;
    
    char* lower_input = strdup(user_input);
    for (char* p = lower_input; *p; ++p) *p = tolower(*p);
    
    // Common state names
    const char* states[] = {"punjab", "haryana", "gujarat", "maharashtra", "rajasthan", 
                           "uttar pradesh", "tamil nadu", "karnataka", "telangana", "andhra pradesh"};
    
    // Look for state names
    for (int i = 0; i < sizeof(states)/sizeof(states[0]); i++) {
        if (strstr(lower_input, states[i])) {
            strcpy(state, states[i]);
            // Capitalize first letter
            state[0] = toupper(state[0]);
            break;
        }
    }
    
    free(lower_input);
}

// Generate response based on intent
char* generate_response(IntentType intent, const char* location, const char* query_details) {
    char* response = malloc(MAX_RESPONSE_LENGTH);
    if (!response) return NULL;
    
    switch (intent) {
        case INTENT_GREETING:
            snprintf(response, MAX_RESPONSE_LENGTH, 
                    "Hello! I'm INGRES, your Groundwater Assessment assistant. I can help you with:\n"
                    "• Groundwater data for any location\n"
                    "• Critical and safe areas identification\n"
                    "• Water resource analysis\n"
                    "• Conservation recommendations\n\n"
                    "Try asking: 'Show me Punjab data' or 'Which areas are critical?'");
            break;
            
        case INTENT_HELP:
            snprintf(response, MAX_RESPONSE_LENGTH,
                    "I can help you with groundwater assessment data. Here are some examples:\n\n"
                    "📍 Location queries:\n"
                    "• 'Show me Punjab groundwater data'\n"
                    "• 'Data for Amritsar district'\n"
                    "• 'Groundwater status in Gujarat'\n\n"
                    "🔍 Category queries:\n"
                    "• 'Which areas are critical?'\n"
                    "• 'Show safe groundwater zones'\n"
                    "• 'Over-exploited regions'\n\n"
                    "📊 Analysis:\n"
                    "• 'Compare Punjab and Haryana'\n"
                    "• 'Database statistics'");
            break;
            
        case INTENT_QUERY_LOCATION:
            {
                char state[50] = "";
                extract_location(location ? location : "", state, NULL, NULL);
                
                if (strlen(state) > 0) {
                    QueryResult* result = query_by_state(state);
                    if (result && result->count > 0) {
                        int safe = 0, critical = 0, over_exploited = 0;
                        for (int i = 0; i < result->count; i++) {
                            if (strcasecmp(result->data[i].category, "Safe") == 0) safe++;
                            else if (strcasecmp(result->data[i].category, "Critical") == 0) critical++;
                            else if (strcasecmp(result->data[i].category, "Over-Exploited") == 0) over_exploited++;
                        }
                        
                        snprintf(response, MAX_RESPONSE_LENGTH,
                                "📊 Groundwater Assessment for **%s**\n\n"
                                "Total districts analyzed: **%d**\n"
                                "✅ Safe areas: **%d**\n"
                                "⚠️ Critical areas: **%d**\n"
                                "🔴 Over-exploited areas: **%d**\n\n"
                                "💡 Data source: Central Ground Water Board (CGWB)\n"
                                "📅 Assessment year: 2024",
                                state, result->count, safe, critical, over_exploited);
                        free_query_result(result);
                    } else {
                        snprintf(response, MAX_RESPONSE_LENGTH,
                                "I couldn't find data for **%s**. Please check the spelling or try a different state.\n\n"
                                "Available states include: Punjab, Haryana, Gujarat, Maharashtra, Rajasthan, Tamil Nadu, Karnataka, Telangana",
                                state);
                    }
                } else {
                    snprintf(response, MAX_RESPONSE_LENGTH,
                            "Please specify a state or district. For example:\n"
                            "• 'Show me Punjab data'\n"
                            "• 'Groundwater status in Maharashtra'");
                }
            }
            break;
            
        case INTENT_CRITICAL_AREAS:
            {
                QueryResult* result = query_critical_areas();
                if (result && result->count > 0) {
                    snprintf(response, MAX_RESPONSE_LENGTH,
                            "🚨 **Critical & Over-Exploited Areas**\n\n"
                            "Found **%d** critical/over-exploited areas:\n\n",
                            result->count);
                    
                    int added = 0;
                    for (int i = 0; i < result->count && i < 5; i++) {
                        char line[200];
                        snprintf(line, sizeof(line), "• **%s, %s** - %s\n",
                                result->data[i].district, result->data[i].state, result->data[i].category);
                        strncat(response, line, MAX_RESPONSE_LENGTH - strlen(response) - 1);
                        added++;
                    }
                    
                    if (result->count > 5) {
                        strncat(response, "\n... and more. Use specific queries for detailed analysis.",
                               MAX_RESPONSE_LENGTH - strlen(response) - 1);
                    }
                    free_query_result(result);
                } else {
                    snprintf(response, MAX_RESPONSE_LENGTH,
                            "No critical areas found in the database.");
                }
            }
            break;
            
        case INTENT_SAFE_AREAS:
            {
                QueryResult* result = query_by_category("Safe");
                if (result && result->count > 0) {
                    snprintf(response, MAX_RESPONSE_LENGTH,
                            "✅ **Safe Groundwater Zones**\n\n"
                            "Found **%d** safe groundwater areas:\n\n",
                            result->count);
                    
                    int added = 0;
                    for (int i = 0; i < result->count && i < 5; i++) {
                        char line[200];
                        snprintf(line, sizeof(line), "• **%s, %s**\n",
                                result->data[i].district, result->data[i].state);
                        strncat(response, line, MAX_RESPONSE_LENGTH - strlen(response) - 1);
                        added++;
                    }
                    free_query_result(result);
                } else {
                    snprintf(response, MAX_RESPONSE_LENGTH,
                            "No safe areas found in the database.");
                }
            }
            break;
            
        case INTENT_SUMMARY_STATISTICS:
            {
                snprintf(response, MAX_RESPONSE_LENGTH,
                        "📊 **Database Statistics**\n\n"
                        "Total Records: **%d**\n\n"
                        "Check console for detailed breakdown of:\n"
                        "• Safe, Critical, and Over-Exploited areas\n"
                        "• State-wise distribution\n"
                        "• Assessment year coverage\n\n"
                        "Use specific queries for detailed location analysis!",
                        sample_data_count);
            }
            break;
            
        case INTENT_GOODBYE:
            snprintf(response, MAX_RESPONSE_LENGTH,
                    "Thank you for using INGRES! Stay informed about groundwater conservation.\n\n"
                    "💧 Remember: Every drop counts!");
            break;
            
        default:
            snprintf(response, MAX_RESPONSE_LENGTH,
                    "I'm not sure how to help with that. Try asking:\n"
                    "• 'Show me data for [state/district]'\n"
                    "• 'Which areas are critical?'\n"
                    "• 'Help' for more options");
            break;
    }
    
    return response;
}

// Process user input and return response
BotResponse* process_user_query(const char* user_input) {
    if (!user_input || strlen(user_input) == 0) {
        BotResponse* response = calloc(1, sizeof(BotResponse));
        response->message = strdup("Please enter a valid query.");
        response->intent = INTENT_UNKNOWN;
        response->confidence_score = 1.0f;
        return response;
    }
    
    // Classify intent
    IntentType intent = classify_intent(user_input);
    
    // Generate response
    char* message = generate_response(intent, user_input, NULL);
    
    // Create response structure
    BotResponse* response = calloc(1, sizeof(BotResponse));
    if (response) {
        response->message = message;
        response->intent = intent;
        response->confidence_score = 0.8f; // Default confidence
        response->has_data = false;
    }
    
    return response;
}

// Free response memory
void free_bot_response(BotResponse* response) {
    if (response) {
        if (response->message) free(response->message);
        if (response->query_result) free_query_result(response->query_result);
        free(response);
    }
}