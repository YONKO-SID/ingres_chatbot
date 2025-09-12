#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "chatbot.h"

// Forward declaration of helper function
const char* get_intent_name(IntentType intent);

// Global chatbot state
static bool chatbot_initialized = false;

// Signal handler for graceful shutdown
void handle_signal(int signal) {
    printf("\n\n[Signal] Received signal %d, shutting down gracefully...\n", signal);
    if (chatbot_initialized) {
        chatbot_cleanup();
        chatbot_initialized = false;
    }
    exit(0);
}

// Print welcome message
void print_welcome(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    INGRES CHATBOT v1.0                     ║\n");
    printf("║                                                              ║\n");
    printf("║  Your AI-powered Groundwater Data Assistant                  ║\n");
    printf("║  Loaded with real groundwater data for India                 ║\n");
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("💡 Try asking me:\n");
    printf("   • \"Show me Punjab data\"\n");
    printf("   • \"Which areas are critical?\"\n");
    printf("   • \"Tell me about safe groundwater areas\"\n");
    printf("   • \"Show me statistics\"\n");
    printf("   • \"Help\" for more options\n\n");
}

// Main interactive loop
void run_interactive_mode(void) {
    char input[512];
    
    printf("🤖 ChatBot is ready! Type 'exit' to quit.\n\n");
    
    while (1) {
        printf("You: ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = '\0';
        
        // Check for exit
        if (strcasecmp(input, "exit") == 0 || 
            strcasecmp(input, "quit") == 0 || 
            strcasecmp(input, "bye") == 0) {
            printf("\n👋 Goodbye! Thank you for using INGRES ChatBot.\n");
            break;
        }
        
        // Process input
        BotResponse* response = process_user_query(input);
        if (response) {
            printf("\n🤖 Bot: %s\n\n", response->message);
            
            // Show confidence if low
            if (response->confidence_score < 0.7f && response->confidence_score > 0.1f) {
                printf("   (Confidence: %.0f%%)\n", response->confidence_score * 100);
            }
            
            free_bot_response(response);
        } else {
            printf("\n❌ Error: Failed to process your query.\n\n");
        }
    }
}

// Run single query mode
void run_single_query_mode(const char* query) {
    printf("Processing query: %s\n\n", query);
    
    BotResponse* response = process_user_query(query);
    if (response) {
        printf("Response: %s\n", response->message);
        printf("Intent: %s\n", get_intent_name(response->intent));
        printf("Confidence: %.2f\n", response->confidence_score);
        
        if (response->has_data) {
            printf("\nData found in response\n");
        }
        
        free_bot_response(response);
    } else {
        printf("Error: Failed to process query\n");
    }
}

// Helper function to get intent name
const char* get_intent_name(IntentType intent) {
    switch (intent) {
        case INTENT_GREETING: return "Greeting";
        case INTENT_HELP: return "Help";
        case INTENT_QUERY_LOCATION: return "Query Location";
        case INTENT_CRITICAL_AREAS: return "Critical Areas";
        case INTENT_SAFE_AREAS: return "Safe Areas";
        case INTENT_SUMMARY_STATISTICS: return "Statistics";
        case INTENT_GOODBYE: return "Goodbye";
        default: return "Unknown";
    }
}

// Test mode with sample queries
void run_test_mode(void) {
    const char* test_queries[] = {
        "Hello",
        "Show me Punjab data",
        "Which areas are critical?",
        "Tell me about safe areas",
        "Show me statistics",
        "Help",
        "Ludhiana district",
        "Delhi groundwater",
        "Critical areas in Maharashtra",
        "What is the data for Rajasthan?"
    };
    
    int num_tests = sizeof(test_queries) / sizeof(test_queries[0]);
    
    printf("🧪 Running %d test queries...\n\n", num_tests);
    
    for (int i = 0; i < num_tests; i++) {
        printf("Test %d: %s\n", i + 1, test_queries[i]);
        printf("--------------------------------------------------\n");
        
        BotResponse* response = process_user_query(test_queries[i]);
        if (response) {
            printf("Response: %s\n", response->message);
            printf("Intent: %s (%.0f%% confidence)\n\n", 
                   get_intent_name(response->intent), response->confidence_score * 100);
            free_bot_response(response);
        }
        
        // Small delay for readability
        #ifdef _WIN32
        Sleep(1000);
        #else
        usleep(500000);
        #endif
    }
    
    printf("✅ All tests completed!\n");
}

// Print usage
void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("OPTIONS:\n");
    printf("  --help, -h        Show this help message\n");
    printf("  --query <text>    Run single query and exit\n");
    printf("  --test            Run test mode with sample queries\n");
    printf("  --interactive     Run in interactive mode (default)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s --query \"Show me Punjab data\"\n", program_name);
    printf("  %s --test\n", program_name);
    printf("  %s\n", program_name);
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Parse command line arguments
    int run_test = 0;
    int run_single = 0;
    char single_query[512] = {0};
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--test") == 0) {
            run_test = 1;
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            run_single = 1;
            strncpy(single_query, argv[i + 1], sizeof(single_query) - 1);
            break;
        } else if (strcmp(argv[i], "--interactive") == 0) {
            // Default mode, do nothing
        }
    }
    
    // Initialize chatbot
    if (!chatbot_init()) {
        fprintf(stderr, "❌ Failed to initialize chatbot\n");
        return 1;
    }
    chatbot_initialized = true;
    
    printf("✅ ChatBot initialized successfully!\n");
    
    // Run in appropriate mode
    if (run_test) {
        run_test_mode();
    } else if (run_single) {
        run_single_query_mode(single_query);
    } else {
        print_welcome();
        run_interactive_mode();
    }
    
    // Cleanup
    chatbot_cleanup();
    chatbot_initialized = false;
    
    return 0;
}