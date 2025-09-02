/*
* INGRES ChatBot - Core Chatbot Header
 */

#ifndef CHATBOT_H
#define CHATBOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Configuration constants
#define MAX_INPUT_LENGTH 512
#define MAX_RESPONSE_LENGTH 2048
#define MAX_LOCATION_LENGTH 100
#define MAX_INTENT_TYPES 10

void userQwery(void);


#endif // CHATBOT_H