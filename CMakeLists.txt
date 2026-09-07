# Set the minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.14)

# Define the project name and specify that it's a C++ project
project(OrderbookProject CXX)

# --- The Fix for Anaconda/Homebrew Conflict ---
# This forces CMake to look for packages in the Homebrew directory first.
# This is critical to ensure we find the native M1/ARM version of Google Test.
set(CMAKE_PREFIX_PATH "/opt/homebrew")

# [cite_start]Set the C++ standard to C++20, to match the original Visual Studio project [cite: 4, 25]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Google Test package (it will now find the Homebrew version)
find_package(GTest REQUIRED)

# --- Define the Orderbook library ---
# This command tells CMake to compile Orderbook.cpp into a static library named "Orderbook"
add_library(Orderbook STATIC
    Orderbook.cpp
)
target_include_directories(Orderbook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Define the Test Executable ---
# This command tells CMake to compile test.cpp into an executable file named "run_tests"
add_executable(run_tests
    OrderbookTest/test.cpp
)
target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Link Libraries ---
target_link_libraries(run_tests PRIVATE Orderbook GTest::gtest GTest::gtest_main)

# --- The Fix for the Runtime Library Loading Error ---
# This command directly embeds the correct runtime path into the final executable,
# telling it to look for the Homebrew libraries when it starts up.
set_target_properties(run_tests PROPERTIES
    BUILD_RPATH "/opt/homebrew/lib"
)

# --- Define the Presentation Executable ---
add_executable(present
    present.cpp
)
target_link_libraries(present PRIVATE Orderbook GTest::gtest GTest::gtest_main)
set_target_properties(present PROPERTIES
    BUILD_RPATH "/opt/homebrew/lib"
)