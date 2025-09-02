/*
 * INGRES ChatBot - Main Entry Point
 * Simple terminal interface to test your chatbot
 */

#include "utils.h"
#include "chatbot.h"

void print_welcome_banner(void) {
    print_colored(COLOR_CYAN, "╔══════════════════════════════════════════════════════════════╗\n");
    print_colored(COLOR_CYAN, "║                    INGRES CHATBOT v1.0                      ║\n");
    print_colored(COLOR_CYAN, "║              AI-Powered Groundwater Assistant                ║\n");
    print_colored(COLOR_CYAN, "║                Smart India Hackathon 2024                   ║\n");
    print_colored(COLOR_CYAN, "╚══════════════════════════════════════════════════════════════╝\n\n");
}

void print_help_commands(void) {
    print_info("Available commands:");
    printf("  • Ask about any location: 'Show me Punjab data'\n");
    printf("  • Compare regions: 'Compare Punjab and Maharashtra'\n");
    printf("  • Find critical areas: 'Show critical areas'\n");
    printf("  • Get help: 'help'\n");
    printf("  • Exit: 'quit' or 'exit'\n\n");
}

int main(void) {
    print_welcome_banner();

    // Initialize chatbot
    print_info("Initializing INGRES ChatBot...");

    if (!chatbot_init()) {
        print_error("Failed to initialize chatbot!");
        return 1;
    }

    print_success("ChatBot initialized successfully!");
    print_help_commands();

    // Main chat loop
    while (true) {
        // Get user input
        string user_input = get_string("🌊 Ask me about groundwater: ");

        // Check for exit commands
        if (!user_input || STR_EQUALS(user_input, "quit") || STR_EQUALS(user_input, "exit")) {
            SAFE_FREE(user_input);
            break;
        }

        // Skip empty input
        if (STR_EMPTY(user_input)) {
            SAFE_FREE(user_input);
            continue;
        }

        // Process the query
        printf("\n");
        print_info("Processing your query...");

        BotResponse* response = process_user_query(user_input);

        if (response) {
            // Print the response
            print_colored(COLOR_GREEN, "🤖 Bot: ");
            printf("%s\n", response->message);

            // Print additional data if available
            if (response->has_data && response->data_points) {
                printf("\n📊 Additional Information:\n");
                for (int i = 0; i < response->data_points->count; i++) {
                    string data_point = string_array_get(response->data_points, i);
                    printf("   • %s\n", data_point);
                }
            }

            // Show processing time
            if (response->processing_time_ms > 0) {
                print_colored(COLOR_YELLOW, "⏱️  Processed in %.2fms\n", response->processing_time_ms);
            }

            free_bot_response(response);
        } else {
            print_error("Sorry, I encountered an error processing your request.");
        }

        printf("\n");
        SAFE_FREE(user_input);
    }

    // Cleanup
    print_info("Shutting down INGRES ChatBot...");
    chatbot_cleanup();
    print_success("Goodbye! Thank you for using INGRES ChatBot!");

    return 0;
}