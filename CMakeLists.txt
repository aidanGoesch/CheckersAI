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

# Optionally, you can specify C++ version and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -lncurses")