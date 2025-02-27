cmake_minimum_required(VERSION 3.27)
project(MONA LANGUAGES C CXX)

set(VERSION "1.5")
set(RELEASE "1")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add each module in order (dependencies first)
add_subdirectory(Mem)
add_subdirectory(Lib)
add_subdirectory(BDD)
add_subdirectory(DFA)
add_subdirectory(GTA)
add_subdirectory(Config)
add_subdirectory(Front)
add_subdirectory(Examples)
add_subdirectory(tests)  # Test executables