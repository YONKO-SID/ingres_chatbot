#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chatbot.h"
#include "utils.h"

// Enhanced test queries showcasing new capabilities
const char* enhanced_test_queries[] = {
    // Basic interaction
    "Hello INGRES",
    "Help me understand groundwater",
    
    // Location queries with fuzzy matching
    "Show me Panjab data",  // Misspelled Punjab
    "Groundwater in Maharashtr",  // Misspelled Maharashtra
    "Tell me about Amritsar district",
    
    // Advanced analysis
    "Compare Punjab vs Haryana groundwater trends",
    "Historical trend for Gujarat over 5 years",
    "Which areas need emergency intervention?",
    
    // Technical queries
    "Explain stage of extraction methodology",
    "What conservation methods work best?",
    "Policy recommendations for over-exploited areas",
    
    // Environmental factors
    "How does monsoon affect groundwater recharge?",
    "Climate change impact on water resources",
    
    // Context-aware follow-ups (to be tested in sequence)
    "Tell me more about that",
    "What are the economic impacts?",
    "Show me success stories",
    
    // Complex queries
    "Show critical areas in northern India with policy recommendations",
    "Agriculture impact on groundwater in Punjab",
    
    "Goodbye"
};

void print_response_details(BotResponse* response) {
    if (!response) return;
    
    printf("🤖 **INGRES RESPONSE**\n");
    printf("Intent: %d | Confidence: %.2f | Time: %.2fms\n", 
           response->intent, response->confidence_score, response->processing_time_ms);
    
    if (response->requires_clarification) {
        printf("⚠️  CLARIFICATION NEEDED: %s\n", response->clarification_question);
    }
    
    printf("\n%s\n", response->message);
    
    if (response->suggestion_count > 0) {
        printf("\n💡 **SUGGESTED FOLLOW-UPS**:\n");
        for (int i = 0; i < response->suggestion_count; i++) {
            printf("   %d. %s\n", i+1, response->suggested_actions[i]);
        }
    }
    
    if (response->source_count > 0) {
        printf("\n📚 **DATA SOURCES**: ");
        for (int i = 0; i < response->source_count; i++) {
            printf("%s", response->data_sources[i]);
            if (i < response->source_count - 1) printf(", ");
        }
        printf("\n");
    }
}

int main() {
    printf("🌊 *** INGRES ChatBot - Enhanced AI System *** 🌊\n");
    printf("================================================\n");
    printf("India's Groundwater Resource Expert System\n");
    printf("Smart India Hackathon 2025 | Enhanced Version\n\n");
    
    // Initialize enhanced chatbot
    if (!chatbot_init()) {
        printf("❌ Failed to initialize INGRES ChatBot!\n");
        return 1;
    }
    
    printf("\n🚀 **ENHANCED FEATURES LOADED**:\n");
    printf("   ✅ 70+ Intent Types with Fuzzy Matching\n");
    printf("   ✅ Context-Aware Conversations\n");
    printf("   ✅ Multi-language Support Framework\n");
    printf("   ✅ Advanced Location Extraction\n");
    printf("   ✅ Confidence Scoring & Clarifications\n");
    printf("   ✅ Follow-up Suggestions\n");
    printf("   ✅ Comprehensive Response Templates\n\n");
    
    // Test enhanced queries
    printf("*** TESTING ENHANCED CAPABILITIES ***\n");
    printf("====================================\n\n");
    
    int num_tests = sizeof(enhanced_test_queries) / sizeof(enhanced_test_queries[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("👤 USER: %s\n", enhanced_test_queries[i]);
        
        // Use enhanced processing
        BotResponse* response = process_user_query(enhanced_test_queries[i]);
        
        if (response) {
            print_response_details(response);
            free_enhanced_bot_response(response);
        } else {
            printf("🤖 BOT: Error processing request\n");
        }
        
        printf("\n" "═══════════════════════════════════════════════════════════════\n\n");
        
        // Add small delay for readability
        #ifdef _WIN32
            system("timeout /t 1 >nul");
        #else
            system("sleep 1");
        #endif
    }
    
    // Interactive enhanced mode
    printf("*** ENHANCED INTERACTIVE MODE ***\n");
    printf("================================\n");
    printf("🎯 Features: Context awareness, fuzzy matching, follow-up suggestions\n");
    printf("💬 Try: 'Show Punjab crisis', then 'tell me more', then 'what solutions?'\n");
    printf("🔤 Test: Misspellings like 'Panjab' or 'Maharashtr'\n");
    printf("❓ Type 'quit' to exit\n\n");
    
    char input[MAX_INPUT_LENGTH];
    int query_number = 1;
    
    while (1) {
        printf("👤 [Q%d]: ", query_number++);
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "bye") == 0) {
            BotResponse* goodbye_response = process_user_query("goodbye");
            if (goodbye_response) {
                print_response_details(goodbye_response);
                free_enhanced_bot_response(goodbye_response);
            }
            break;
        }
        
        if (strlen(input) == 0) {
            printf("🤖 Please enter a query or 'quit' to exit.\n\n");
            continue;
        }
        
        // Process with enhanced system
        BotResponse* response = process_user_query(input);
        
        if (response) {
            print_response_details(response);
            free_enhanced_bot_response(response);
        } else {
            printf("🤖 Sorry, I encountered an error processing your request.\n");
        }
        
        printf("\n");
    }
    
    // Cleanup
    chatbot_cleanup();
    printf("\n🎉 Thank you for testing INGRES ChatBot Enhanced System!\n");
    printf("💡 Ready for Smart India Hackathon 2025!\n");
    
    return 0;
}