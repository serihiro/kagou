cmake_minimum_required(VERSION 3.12)
project(kagou C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -O2")

# Enable AddressSanitizer for debug builds
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")

# Source files
file(GLOB_RECURSE SOURCES "src/*.c")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Main executable
add_executable(kagou src/main.c ${SOURCES})
target_link_libraries(kagou OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(kagou PRIVATE src)

# Create library for testing
add_library(kagou_lib STATIC ${SOURCES})
target_include_directories(kagou_lib PRIVATE src)
target_link_libraries(kagou_lib OpenSSL::SSL OpenSSL::Crypto)

# Test executables
file(GLOB TEST_SOURCES "tests/*.c")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} kagou_lib)
    target_include_directories(${TEST_NAME} PRIVATE src)
endforeach()

# Custom target to run all tests
add_custom_target(test_all
    COMMAND ${CMAKE_COMMAND} -E echo "Running all tests..."
    DEPENDS ${TEST_SOURCES}
)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_custom_command(TARGET test_all POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running ${TEST_NAME}..."
        COMMAND ./${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()