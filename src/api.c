// REST API endpoints
void handle_chat_request(const char* message, char** response) {
    BotResponse* bot_response = process_user_query(message);
    // Convert to JSON and return
}
