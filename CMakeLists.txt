cmake_minimum_required(VERSION 3.0)
project(MyCheckersGame)

# Add executable
add_executable(checkers_game main.cpp)

# Include directory for header files
target_include_directories(checkers_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/checkers_board)

# Add the source files from the checkers_board directory
target_sources(checkers_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/checkers_board/checkers_board.cpp
)

# Specify C++ version and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -lncurses")

# Add debug symbols for Valgrind
set(CMAKE_BUILD_TYPE Debug)

# Find Valgrind
find_program(VALGRIND "valgrind")

if(VALGRIND)
    # Add a custom target for running the program with Valgrind
    add_custom_target(valgrind
        COMMAND ${VALGRIND} --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose $<TARGET_FILE:checkers_game>
        DEPENDS checkers_game
    )

    # Enable testing
    enable_testing()

    # Add a test that runs the program with Valgrind
    add_test(NAME valgrind_test
        COMMAND ${VALGRIND} --leak-check=full --error-exitcode=1 $<TARGET_FILE:checkers_game>
    )
else()
    message(WARNING "Valgrind not found. Valgrind target will not be available.")
endif()