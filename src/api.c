#include "chatbot.h"
#include "../lib/mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>  // For JSON handling

// API endpoint handlers
static void handle_chat_endpoint(struct mg_connection *c, struct mg_http_message *hm);
static void handle_status_endpoint(struct mg_connection *c, struct mg_http_message *hm);
static void handle_health_endpoint(struct mg_connection *c, struct mg_http_message *hm);
static void handle_capabilities_endpoint(struct mg_connection *c, struct mg_http_message *hm);

// Convert BotResponse to JSON
char* bot_response_to_json(BotResponse* response) {
    if (!response) return NULL;
    
    json_object *root = json_object_new_object();
    json_object *message = json_object_new_string(response->message ? response->message : "");
    json_object *intent = json_object_new_int(response->intent);
    json_object *confidence = json_object_new_double(response->confidence_score);
    json_object *processing_time = json_object_new_double(response->processing_time_ms);
    json_object *has_data = json_object_new_boolean(response->has_data);
    json_object *requires_clarification = json_object_new_boolean(response->requires_clarification);
    
    json_object_object_add(root, "message", message);
    json_object_object_add(root, "intent", intent);
    json_object_object_add(root, "confidence", confidence);
    json_object_object_add(root, "processing_time_ms", processing_time);
    json_object_object_add(root, "has_data", has_data);
    json_object_object_add(root, "requires_clarification", requires_clarification);
    
    // Add suggestions array
    if (response->suggestion_count > 0) {
        json_object *suggestions = json_object_new_array();
        for (int i = 0; i < response->suggestion_count; i++) {
            json_object *suggestion = json_object_new_string(response->suggested_actions[i]);
            json_object_array_add(suggestions, suggestion);
        }
        json_object_object_add(root, "suggestions", suggestions);
    }
    
    // Add clarification question if needed
    if (response->clarification_question) {
        json_object *clarification = json_object_new_string(response->clarification_question);
        json_object_object_add(root, "clarification_question", clarification);
    }
    
    // Add data sources
    if (response->source_count > 0) {
        json_object *sources = json_object_new_array();
        for (int i = 0; i < response->source_count; i++) {
            json_object *source = json_object_new_string(response->data_sources[i]);
            json_object_array_add(sources, source);
        }
        json_object_object_add(root, "data_sources", sources);
    }
    
    const char *json_string = json_object_to_json_string(root);
    char *result = strdup(json_string);
    
    json_object_put(root);  // Free JSON object
    return result;
}

// Main HTTP event handler
static void http_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Enable CORS
        mg_printf(c, "HTTP/1.1 200 OK\r\n"
                     "Access-Control-Allow-Origin: *\r\n"
                     "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                     "Access-Control-Allow-Headers: Content-Type\r\n"
                     "Content-Type: application/json\r\n\r\n");
        
        // Route requests
        if (mg_http_match_uri(hm, "/api/chat")) {
            handle_chat_endpoint(c, hm);
        } else if (mg_http_match_uri(hm, "/api/status")) {
            handle_status_endpoint(c, hm);
        } else if (mg_http_match_uri(hm, "/api/health")) {
            handle_health_endpoint(c, hm);
        } else if (mg_http_match_uri(hm, "/api/capabilities")) {
            handle_capabilities_endpoint(c, hm);
        } else {
            // Serve static files or 404
            struct mg_http_serve_opts opts = {.root_dir = "./web"};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

// Chat endpoint handler
static void handle_chat_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    if (mg_vcmp(&hm->method, "POST") != 0) {
        mg_printf(c, "{\"error\": \"Method not allowed\"}\n");
        return;
    }
    
    // Parse JSON request
    json_object *root = json_tokener_parse(hm->body.ptr);
    if (!root) {
        mg_printf(c, "{\"error\": \"Invalid JSON\"}\n");
        return;
    }
    
    json_object *message_obj;
    if (!json_object_object_get_ex(root, "message", &message_obj)) {
        mg_printf(c, "{\"error\": \"Missing message field\"}\n");
        json_object_put(root);
        return;
    }
    
    const char *user_message = json_object_get_string(message_obj);
    
    // Process with enhanced chatbot
    BotResponse* response = process_user_query(user_message);
    
    if (response) {
        char* json_response = bot_response_to_json(response);
        if (json_response) {
            mg_printf(c, "%s\n", json_response);
            free(json_response);
        } else {
            mg_printf(c, "{\"error\": \"Failed to generate response\"}\n");
        }
        free_enhanced_bot_response(response);
    } else {
        mg_printf(c, "{\"error\": \"Internal server error\"}\n");
    }
    
    json_object_put(root);
}

// Status endpoint handler
static void handle_status_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    json_object *root = json_object_new_object();
    json_object *status = json_object_new_string("online");
    json_object *version = json_object_new_string("2.0.0-enhanced");
    json_object *capabilities = json_object_new_int(70);  // 70+ intents
    json_object *uptime = json_object_new_int(time(NULL));  // Simple uptime
    
    json_object_object_add(root, "status", status);
    json_object_object_add(root, "version", version);
    json_object_object_add(root, "intent_count", capabilities);
    json_object_object_add(root, "server_time", uptime);
    
    const char *json_string = json_object_to_json_string(root);
    mg_printf(c, "%s\n", json_string);
    
    json_object_put(root);
}

// Health check endpoint
static void handle_health_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    mg_printf(c, "{\"status\": \"healthy\", \"timestamp\": %ld}\n", time(NULL));
}

// Capabilities endpoint
static void handle_capabilities_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    json_object *root = json_object_new_object();
    
    // Create capabilities array
    json_object *capabilities = json_object_new_array();
    
    const char* capability_list[] = {
        "Location-based groundwater queries",
        "Historical trend analysis",
        "Multi-location comparisons", 
        "Policy recommendations",
        "Conservation method suggestions",
        "Crisis area identification",
        "Technical explanations",
        "Context-aware conversations",
        "Fuzzy string matching",
        "Multi-language support framework",
        "Real-time confidence scoring",
        "Follow-up suggestions",
        "Data source attribution"
    };
    
    int cap_count = sizeof(capability_list) / sizeof(capability_list[0]);
    for (int i = 0; i < cap_count; i++) {
        json_object *cap = json_object_new_string(capability_list[i]);
        json_object_array_add(capabilities, cap);
    }
    
    json_object_object_add(root, "capabilities", capabilities);
    json_object_object_add(root, "total_intents", json_object_new_int(70));
    json_object_object_add(root, "supported_languages", json_object_new_string("English, Hindi (framework)"));
    
    const char *json_string = json_object_to_json_string(root);
    mg_printf(c, "%s\n", json_string);
    
    json_object_put(root);
}

// Start API server
int start_api_server(const char* port) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    
    mg_mgr_init(&mgr);
    
    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "http://0.0.0.0:%s", port);
    
    c = mg_http_listen(&mgr, listen_addr, http_handler, NULL);
    if (c == NULL) {
        printf("❌ Failed to start API server on port %s\n", port);
        return 1;
    }
    
    printf("🌐 INGRES API Server started on http://localhost:%s\n", port);
    printf("📡 Endpoints available:\n");
    printf("   POST /api/chat - Main chat interface\n");
    printf("   GET  /api/status - Server status\n");
    printf("   GET  /api/health - Health check\n");
    printf("   GET  /api/capabilities - System capabilities\n");
    printf("   GET  / - Static web interface\n\n");
    
    // Event loop
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}

// Legacy function for backward compatibility
void handle_chat_request(const char* message, char** response) {
    BotResponse* bot_response = process_user_query(message);
    if (bot_response && bot_response->message) {
        *response = strdup(bot_response->message);
        free_enhanced_bot_response(bot_response);
    } else {
        *response = strdup("Error processing request");
    }
}
