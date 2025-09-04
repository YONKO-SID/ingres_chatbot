#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chatbot.h"
#include "utils.h"

// Simple test queries for demonstration
const char* test_queries[] = {
    "Hello",
    "Show me Punjab groundwater data",
    "Which areas are critical?",
    "Water crisis areas",
    "Policy suggestions for Maharashtra", 
    "How does rainfall affect groundwater?",
    "Compare Punjab and Haryana",
    "Safe areas in India",
    "Help",
    "Bot status",
    "Bye"
};

int main() {
    printf("*** INGRES ChatBot - Quick Test Program ***\n");
    printf("==========================================\n\n");
    
    // Initialize chatbot
    if (!chatbot_init()) {
        printf("❌ Failed to initialize chatbot!\n");
        return 1;
    }
    
    printf("[OK] ChatBot initialized successfully!\n\n");
    
    // Test predefined queries
    printf("*** TESTING PREDEFINED QUERIES ***\n");
    printf("==================================\n\n");
    
    int num_tests = sizeof(test_queries) / sizeof(test_queries[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("USER: %s\n", test_queries[i]);
        printf("BOT:  ");
        
        // Generate response using our function
        IntentType intent = classify_intent(test_queries[i]);
        char* response = generate_response(intent, "Punjab", test_queries[i]);
        
        if (response) {
            printf("%s\n", response);
            free(response);
        } else {
            printf("Error generating response\n");
        }
        
        printf("\n----------------------------------------\n\n");
    }
    
    // Interactive mode
    printf("*** INTERACTIVE MODE ***\n");
    printf("========================\n");
    printf("Type your queries (or 'quit' to exit):\n\n");
    
    char input[MAX_INPUT_LENGTH];
    while (1) {
        printf("YOU: ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            printf("BOT: Thank you for testing INGRES ChatBot! Goodbye!\n");
            break;
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        IntentType intent = classify_intent(input);
        char* response = generate_response(intent, "TestLocation", input);
        
        if (response) {
            printf("BOT: %s\n\n", response);
            free(response);
        } else {
            printf("BOT: Sorry, I encountered an error processing your request.\n\n");
        }
    }
    
    // Cleanup
    chatbot_cleanup();
    printf("\n[OK] ChatBot cleanup completed. Goodbye!\n");
    
    return 0;
}