cmake_minimum_required(VERSION 3.10)
project(ingres_chatbot C)

# Add the 'include' directory to the list of directories to search for header files
include_directories(include lib)

# Define the executable and list all source files
# Make sure to list all your .c files here
add_executable(ingres_chatbot
        src/main.c
        src/utils.c
        src/database.c # This file needs to be created
        lib/mongoose.c
        src/chatbot.c  # This file needs the core logic
)

# Link any necessary libraries (e.g., for GLib if you use it, or math library)
# If you are using GLib, you'll need to find_package(PkgConfig) and then pkg_check_modules(GLIB REQUIRED glib-2.0)
# Then link_libraries(${GLIB_LIBRARIES})
# For now, just the math library if you use math functions
target_link_libraries(ingres_chatbot m ws2_32)

# Set C standard (optional, but good practice)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)