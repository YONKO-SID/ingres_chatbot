cmake_minimum_required(VERSION 4.0)
project(ingres_chatbot C)

set(CMAKE_C_STANDARD 11)

add_executable(ingres_chatbot
        chatbot.c
        include/chatbot.h
        include/database.h
        include/utils.h
        src/utils.c
        src/main.c)
