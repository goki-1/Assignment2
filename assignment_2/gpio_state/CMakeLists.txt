# CMake Build Configuration for root of project
cmake_minimum_required(VERSION 3.18)
project(gpio_statemachine_demo 
    VERSION 1.0 
    DESCRIPTION "GPIO State Machine" 
    LANGUAGES C)

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Werror -Wpedantic -Wextra -D_POSIX_C_SOURCE=200809L -Wshadow)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer
# (Comment this out to make your code faster)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

# Enable PThread library for linking
add_compile_options(-pthread)
add_link_options(-pthread)




# Build the app
include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(gpio_statemachine ${MY_SOURCES})
 
# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET gpio_statemachine POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:gpio_statemachine>"
     "~/cmpt433/public/myApps/gpio_statemachine" 
  COMMENT "Copying ARM executable to public NFS directory")


# Support GPIO
find_library(GPIOD_LIBRARY gpiod)       # UNSURE IF NEEDED
target_link_libraries(gpio_statemachine LINK_PRIVATE gpiod) # May need to change to HAL lib.
