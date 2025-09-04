#include "chatbot.h"
#include "utils.h"
#include "database.h"
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


// Leviathan algorithm implementation for fuzzy matching
typedef struct {
    char* pattern;
    float threshold;
    int max_matches;
} FuzzyMatcher;

typedef struct {
    char* pattern;
    int max_matches;
} SimpleMatcher;
typedef struct {
    char* pattern;
    int max_matches;
} RegexMatcher;
typedef struct {
    char* pattern;
    int max_matches;
} LevenshteinMatcher;
bool chatbot_init(void);
void chatbot_cleanup(void);
IntentType classify_intent(const char* user_input);
// simple levenshtein distance implementation
int levenshtein_distance(const char* str1, const char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    // Allocate memory on the heap to prevent stack overflow for long strings
    int** matrix = malloc((len1 + 1) * sizeof(int*));
    if (matrix == NULL) return -1; // Allocation failed
    for (int i = 0; i <= len1; i++) {
        matrix[i] = malloc((len2 + 1) * sizeof(int));
        if (matrix[i] == NULL) {
            // Cleanup on partial allocation failure
            for (int k = 0; k < i; k++) free(matrix[k]);
            free(matrix);
            return -1;
        }
    }

    for (int i = 0; i <= len1; i++)
        matrix[i][0] = i;
    for (int j = 0; j <= len2; j++)
        matrix[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            matrix[i][j] = (str1[i - 1] == str2[j - 1]) ?
                matrix[i - 1][j - 1] :
                1 + fmin(matrix[i - 1][j], fmin(matrix[i][j - 1], matrix[i - 1][j - 1]));
        }
    }

    int result = matrix[len1][len2];

    // Free the allocated memory
    for (int i = 0; i <= len1; i++) free(matrix[i]);
    free(matrix);

    return result;
}

bool chatbot_init(void) {
    // Initializing the database
    if (!db_init()) {
        print_error("Failed to initialize database!");
        return false;
    }
    return true;
}

void chatbot_cleanup(void) {
    // Closing the database
    db_close();
}

IntentType classify_intent(const char* user_input) {
    // Convert to lowercase for better matching
    char* lower_input = string_to_lower(user_input);
    if (!lower_input) return INTENT_ERROR;
    
    // Greeting patterns - check first
    if (strstr(lower_input, "hello") || strstr(lower_input, "hi") || 
        strstr(lower_input, "namaste") || strstr(lower_input, "hey") ||
        strstr(lower_input, "good morning") || strstr(lower_input, "good evening") ||
        strstr(lower_input,"good night")|| strstr(lower_input,"pranam")) {
        free(lower_input);
        return INTENT_GREETING;
    }
    
    // Goodbye patterns
    if (strstr(lower_input, "bye") || strstr(lower_input, "goodbye") || 
        strstr(lower_input, "exit") || strstr(lower_input, "quit") ||
        strstr(lower_input, "see you") || strstr(lower_input, "thanks")) {
        free(lower_input);
        return INTENT_GOODBYE;
    }
    
    // Help patterns - very specific
    if (strstr(lower_input, "help") || strstr(lower_input, "how to use") ||
        strstr(lower_input, "what can you do") || strstr(lower_input, "commands") ||
        strstr(lower_input, "options") || strstr(lower_input, "guide")) {
        free(lower_input);
        return INTENT_HELP;
    }
    
    // Status patterns - very specific
    if ((strstr(lower_input, "status") && strstr(lower_input, "bot")) ||
        (strstr(lower_input, "system") && strstr(lower_input, "status")) ||
        strstr(lower_input, "bot status") || strstr(lower_input, "chatbot status")) {
        free(lower_input);
        return INTENT_STATUS;
    }
    
    // Query category patterns - specific category searches
    if ((strstr(lower_input, "show") && (strstr(lower_input, "over-exploited") || strstr(lower_input, "semi-critical"))) ||
        (strstr(lower_input, "list") && (strstr(lower_input, "critical") || strstr(lower_input, "safe"))) ||
        strstr(lower_input, "category") || strstr(lower_input, "classification")) {
        free(lower_input);
        return INTENT_QUERY_CATEGORY;
    }
    
    // Critical areas patterns - enhanced (check before other patterns)
    if (strstr(lower_input, "critical areas") || strstr(lower_input, "critical regions") ||
        strstr(lower_input, "over-exploited") || strstr(lower_input, "over exploited") ||
        (strstr(lower_input, "which") && strstr(lower_input, "critical")) ||
        (strstr(lower_input, "which") && strstr(lower_input, "serious")) ||
        (strstr(lower_input, "show") && strstr(lower_input, "critical")) ||
        (strstr(lower_input, "areas") && strstr(lower_input, "critical")) ||
        (strstr(lower_input, "areas") && strstr(lower_input, "serious")) ||
        strstr(lower_input, "dangerous areas") || strstr(lower_input, "problem areas") ||
        strstr(lower_input, "critical groundwater")) {
        free(lower_input);
        return INTENT_CRITICAL_AREAS;
    }
    
    // Safe areas patterns - enhanced
    if ((strstr(lower_input, "safe") && (strstr(lower_input, "area") || strstr(lower_input, "region") || strstr(lower_input, "zone"))) ||
        (strstr(lower_input, "which") && strstr(lower_input, "safe")) ||
        (strstr(lower_input, "show") && strstr(lower_input, "safe")) ||
        strstr(lower_input, "sustainable areas") || strstr(lower_input, "good areas")) {
        free(lower_input);
        return INTENT_SAFE_AREAS;
    }
    
    // Water crisis patterns - enhanced
    if (strstr(lower_input, "water crisis") || strstr(lower_input, "water shortage") ||
        strstr(lower_input, "water stress") || strstr(lower_input, "drought") ||
        strstr(lower_input, "water scarcity") || strstr(lower_input, "water problem") ||
        (strstr(lower_input, "crisis") && strstr(lower_input, "area"))) {
        free(lower_input);
        return INTENT_WATER_CRISIS;
    }
    
    // Rainfall correlation patterns - enhanced
    if (strstr(lower_input, "rainfall") || strstr(lower_input, "monsoon") ||
        strstr(lower_input, "precipitation") || strstr(lower_input, "rain") ||
        (strstr(lower_input, "how") && strstr(lower_input, "rain")) ||
        strstr(lower_input, "weather impact") || strstr(lower_input, "climate")) {
        free(lower_input);
        return INTENT_RAINFALL_CORRELATION;
    }
    
    // Policy suggestion patterns - enhanced
    if (strstr(lower_input, "policy") || strstr(lower_input, "suggestion") ||
        strstr(lower_input, "recommendation") || strstr(lower_input, "what should") ||
        strstr(lower_input, "how to improve") || strstr(lower_input, "solution") ||
        strstr(lower_input, "measures") || strstr(lower_input, "action")) {
        free(lower_input);
        return INTENT_POLICY_SUGGESTION;
    }
    
    // Compare locations patterns - enhanced
    if (strstr(lower_input, "compare") || strstr(lower_input, "vs") ||
        strstr(lower_input, "versus") || strstr(lower_input, "difference between") ||
        strstr(lower_input, "better than") || strstr(lower_input, "worse than") ||
        (strstr(lower_input, "and") && (strstr(lower_input, "punjab") || strstr(lower_input, "haryana")))) {
        free(lower_input);
        return INTENT_COMPARE_LOCATIONS;
    }
    
    // Historical trend patterns - enhanced
    if (strstr(lower_input, "trend") || strstr(lower_input, "historical") ||
        strstr(lower_input, "past data") || strstr(lower_input, "over time") ||
        strstr(lower_input, "history") || strstr(lower_input, "previous") ||
        strstr(lower_input, "change over") || strstr(lower_input, "timeline")) {
        free(lower_input);
        return INTENT_HISTORICAL_TREND;
    }
    
    // Location query patterns - most flexible, check last
    if (strstr(lower_input, "show") || strstr(lower_input, "data for") || 
        strstr(lower_input, "groundwater in") || strstr(lower_input, "display") ||
        strstr(lower_input, "tell me about") || strstr(lower_input, "information about") ||
        strstr(lower_input, "status of") || strstr(lower_input, "what is") ||
        strstr(lower_input, "punjab") || strstr(lower_input, "haryana") || 
        strstr(lower_input, "rajasthan") || strstr(lower_input, "gujarat") ||
        strstr(lower_input, "maharashtra") || strstr(lower_input, "karnataka") ||
        strstr(lower_input, "tamil nadu") || strstr(lower_input, "kerala") ||
        strstr(lower_input, "delhi") || strstr(lower_input, "mumbai") ||
        strstr(lower_input, "bangalore") || strstr(lower_input, "chennai") ||
        strstr(lower_input, "kolkata") || strstr(lower_input, "hyderabad") ||
        strstr(lower_input, "amritsar") || strstr(lower_input, "ludhiana") ||
        strstr(lower_input, "jaipur") || strstr(lower_input, "ahmedabad")) {
        free(lower_input);
        return INTENT_QUERY_LOCATION;
    }
    
    free(lower_input);
    return INTENT_UNKNOWN;
}
// Simplified process_user_input for testing
BotResponse* process_user_input(const char* user_input) {
    BotResponse* response = malloc(sizeof(BotResponse));
    if (!response) return NULL;
    
    IntentType intent = classify_intent(user_input);
    response->intent = intent;
    response->message = generate_response(intent, "TestLocation", user_input);
    response->query_result = NULL;
    response->has_data = false;
    response->processing_time_ms = 0.0f;
    
    return response;
}

char* generate_response(IntentType intent, const char* location, const char* query_details) {
    char* response = malloc(MAX_RESPONSE_LENGTH);
    if (!response) return NULL;
    
    switch (intent) {
        case INTENT_GREETING:
            snprintf(response, MAX_RESPONSE_LENGTH, 
                "Namaste! I'm your INGRES groundwater assistant. I can help you with:\n"
                "* Groundwater data for any location\n"
                "* Critical and over-exploited areas\n"
                "* Historical trends and comparisons\n"
                "* Policy recommendations\n\n"
                "Try asking: 'Show me Punjab groundwater data' or 'Which areas are critical?'");
            break;
            
        case INTENT_GOODBYE:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "Thank you for using INGRES ChatBot! Stay informed about groundwater resources.");
            break;
            
        case INTENT_HELP:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "INGRES ChatBot Help:\n\n"
                "[LOC] Location Queries: 'Show me [state/district] data'\n"
                "[CRIT] Critical Areas: 'Show critical areas' or 'Over-exploited regions'\n"
                "[COMP] Comparisons: 'Compare Punjab and Haryana'\n"
                "[TREND] Trends: 'Historical trend for Maharashtra'\n"
                "[CRISIS] Water Crisis: 'Areas facing water shortage'\n"
                "[RAIN] Rainfall Impact: 'How does rainfall affect groundwater?'\n"
                "[POLICY] Policy Help: 'Policy suggestions for [location]'\n\n"
                "Example: 'What is the groundwater status of Amritsar?'");
            break;
            
        case INTENT_QUERY_LOCATION:
            if (location && strlen(location) > 0) {
                // This will be enhanced when we connect to actual database
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "[SEARCH] Searching groundwater data for %s...\n\n"
                    "Based on latest CGWB assessment:\n"
                    "* Location: %s\n"
                    "* Assessment Year: 2023\n"
                    "* Status: [Fetching from database...]\n"
                    "* Stage of Extraction: [Calculating...]\n\n"
                    "Note: Connecting to INGRES database for real-time data...", 
                    location, location);
            } else {
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "Please specify a location. Example:\n"
                    "* 'Show me Punjab groundwater data'\n"
                    "* 'Groundwater status of Amritsar'\n"
                    "* 'Data for Maharashtra'");
            }
            break;
            
        case INTENT_CRITICAL_AREAS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[ALERT] CRITICAL GROUNDWATER AREAS (Latest Assessment):\n\n"
                "[OVER-EXPLOITED] (>100%% extraction):\n"
                "* Punjab: Amritsar, Ludhiana, Bathinda\n"
                "* Haryana: Sirsa, Hisar\n"
                "* Gujarat: Ahmedabad City\n"
                "* Tamil Nadu: Chennai\n\n"
                "[CRITICAL] (90-100%% extraction):\n"
                "* Maharashtra: Pune, Aurangabad\n"
                "* Karnataka: Bangalore Urban\n"
                "* Rajasthan: Alwar\n\n"
                "NOTE: These areas need immediate water conservation measures!");
            break;
            
        case INTENT_QUERY_CATEGORY:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[CATEGORY] GROUNDWATER CLASSIFICATION SYSTEM:\n\n"
                "CGWB CATEGORIES (Based on Stage of Extraction):\n\n"
                "[SAFE] <70%% extraction:\n"
                "* Sustainable groundwater usage\n"
                "* No restrictions on new bore wells\n"
                "* Examples: Jaipur, Mysore, Kolkata\n\n"
                "[SEMI-CRITICAL] 70-90%% extraction:\n"
                "* Moderate stress on groundwater\n"
                "* Regulated development needed\n"
                "* Examples: Kurukshetra, Nashik, Coimbatore\n\n"
                "[CRITICAL] 90-100%% extraction:\n"
                "* High stress, immediate action needed\n"
                "* Strict regulations required\n"
                "* Examples: Pune, Bangalore, Alwar\n\n"
                "[OVER-EXPLOITED] >100%% extraction:\n"
                "* Groundwater mining occurring\n"
                "* Ban on new extractions\n"
                "* Examples: Amritsar, Ludhiana, Chennai");
            break;
            
        case INTENT_SAFE_AREAS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[SAFE] SAFE GROUNDWATER AREAS (<70%% extraction):\n\n"
                "[SAFE REGIONS]:\n"
                "* Rajasthan: Jaipur, Bikaner\n"
                "* Gujarat: Vadodara\n"
                "* Maharashtra: Mumbai Suburban\n"
                "* Tamil Nadu: Madurai\n"
                "* Karnataka: Mysore\n"
                "* West Bengal: Kolkata, Howrah\n"
                "* Andhra Pradesh: Visakhapatnam\n\n"
                "NOTE: These areas have sustainable groundwater usage patterns.");
            break;
            
        case INTENT_WATER_CRISIS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[CRISIS] WATER CRISIS ALERT AREAS:\n\n"
                "IMMEDIATE ATTENTION NEEDED:\n"
                "* Punjab (Central): 78%% blocks over-exploited\n"
                "* Haryana (Southwest): Rapid depletion rate\n"
                "* Tamil Nadu (Chennai): Urban water stress\n"
                "* Karnataka (Bangalore): IT sector impact\n\n"
                "CRISIS INDICATORS:\n"
                "* Declining water table: 0.5-2m annually\n"
                "* Industrial over-extraction\n"
                "* Intensive agriculture in arid regions\n\n"
                "URGENT ACTIONS: Rainwater harvesting, drip irrigation, policy enforcement");
            break;
            
        case INTENT_RAINFALL_CORRELATION:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[RAINFALL] RAINFALL-GROUNDWATER CORRELATION:\n\n"
                "MONSOON IMPACT:\n"
                "* Good Monsoon (>110%% normal): +15-25%% recharge\n"
                "* Normal Monsoon (90-110%%): Stable recharge\n"
                "* Poor Monsoon (<90%%): -20-40%% recharge\n\n"
                "REGIONAL PATTERNS:\n"
                "* Western Ghats: High recharge efficiency (60-80%%)\n"
                "* Gangetic Plains: Moderate efficiency (40-60%%)\n"
                "* Arid Regions: Low efficiency (10-30%%)\n\n"
                "NOTE: Climate change is affecting traditional recharge patterns!");
            break;
            
        case INTENT_POLICY_SUGGESTION:
            if (location && strlen(location) > 0) {
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "[POLICY] POLICY RECOMMENDATIONS FOR %s:\n\n"
                    "IMMEDIATE MEASURES:\n"
                    "* Mandatory rainwater harvesting for buildings >300 sq.m\n"
                    "* Groundwater extraction permits with annual limits\n"
                    "* Subsidies for drip irrigation systems\n\n"
                    "MEDIUM-TERM STRATEGIES:\n"
                    "* Crop diversification from water-intensive crops\n"
                    "* Industrial water recycling mandates\n"
                    "* Community-based water management\n\n"
                    "MONITORING:\n"
                    "* Real-time groundwater level monitoring\n"
                    "* Annual assessment updates\n"
                    "* Public awareness campaigns", location);
            } else {
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "[POLICY] GENERAL GROUNDWATER POLICY FRAMEWORK:\n\n"
                    "NATIONAL LEVEL:\n"
                    "* National Water Policy implementation\n"
                    "* CGWB guidelines enforcement\n"
                    "* Inter-state water sharing agreements\n\n"
                    "STATE LEVEL:\n"
                    "* Groundwater regulation acts\n"
                    "* Water conservation incentives\n"
                    "* Agricultural policy reforms\n\n"
                    "Specify a location for targeted recommendations!");
            }
            break;
            
        case INTENT_COMPARE_LOCATIONS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[COMPARE] GROUNDWATER COMPARISON:\n\n"
                "PUNJAB vs HARYANA (Example Comparison):\n\n"
                "PUNJAB:\n"
                "* Over-exploited: 78%% of blocks\n"
                "* Stage of Extraction: >150%% in many areas\n"
                "* Main Issue: Intensive rice-wheat cultivation\n"
                "* Water Table: Declining 0.5-1m annually\n\n"
                "HARYANA:\n"
                "* Over-exploited: 45%% of blocks\n"
                "* Stage of Extraction: 90-120%% in critical areas\n"
                "* Main Issue: Industrial + agricultural demand\n"
                "* Water Table: Mixed trends by region\n\n"
                "RECOMMENDATION: Both states need urgent water conservation measures.\n"
                "Specify exact locations for detailed comparison.");
            break;
            
        case INTENT_HISTORICAL_TREND:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[TREND] HISTORICAL GROUNDWATER TRENDS:\n\n"
                "NATIONAL TRENDS (2017-2023):\n\n"
                "DECLINING AREAS:\n"
                "* Punjab: Water table dropped 2-3m\n"
                "* Haryana: 15%% increase in over-exploited blocks\n"
                "* Tamil Nadu: Chennai crisis worsened\n"
                "* Karnataka: Bangalore stress increased\n\n"
                "IMPROVING AREAS:\n"
                "* Rajasthan: Better monsoon management\n"
                "* Gujarat: Successful water conservation\n"
                "* Maharashtra: Watershed programs effective\n\n"
                "KEY FACTORS:\n"
                "* Climate change impact\n"
                "* Policy interventions\n"
                "* Agricultural practices\n\n"
                "Specify a location for detailed trend analysis.");
            break;
            
        case INTENT_STATUS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[STATUS] INGRES CHATBOT STATUS:\n\n"
                "[OK] System: Online and operational\n"
                "[OK] Database: Connected to sample data\n"
                "[OK] Coverage: Pan-India groundwater data\n"
                "[OK] Languages: English (Hindi support coming soon)\n"
                "[OK] Last Update: 2023 CGWB Assessment\n\n"
                "CAPABILITIES:\n"
                "* 6000+ assessment units covered\n"
                "* Real-time query processing\n"
                "* Historical trend analysis\n"
                "* Policy recommendation engine");
            break;
            
        case INTENT_UNKNOWN:
        default:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[HELP] I didn't quite understand that. I can help with:\n\n"
                "* 'Show me [location] groundwater data'\n"
                "* 'Which areas are critical?'\n"
                "* 'Compare [state1] and [state2]'\n"
                "* 'Policy suggestions for [location]'\n"
                "* 'Areas facing water crisis'\n\n"
                "Try rephrasing your question or type 'help' for more options.");
            break;
    }
    
    return response;
}


void free_bot_response(BotResponse* response) {
    if (!response) return;
    
    if (response->message) {
        free(response->message);
    }
    
    if (response->query_result) {
        free_query_result(response->query_result);
    }
    
    free(response);
}