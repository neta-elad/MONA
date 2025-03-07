cmake_minimum_required(VERSION 3.27)
project(MONA LANGUAGES C CXX)

set(VERSION "1.5")
set(RELEASE "1")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Werror)

# Set build type
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Add each module in order (dependencies first)
add_subdirectory(Mem)
add_subdirectory(Lib)
add_subdirectory(BDD)
add_subdirectory(DFA)
add_subdirectory(GTA)
add_subdirectory(Config)
add_subdirectory(Front)
add_subdirectory(Frontlib)
add_subdirectory(Bin)
add_subdirectory(Examples)
add_subdirectory(tests)  # Test executables
add_subdirectory(pymona)