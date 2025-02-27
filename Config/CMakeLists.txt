# Config/CMakeLists.txt

include(CheckIncludeFile)
include(CheckFunctionExists)

# Check for header files
check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)

# Check for functions
check_function_exists(alarm HAVE_ALARM)
check_function_exists(getpagesize HAVE_GETPAGESIZE)
check_function_exists(mmap HAVE_MMAP)

# Generate config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

# Make the generated config.h available
add_library(config INTERFACE)
target_include_directories(config INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

# Provide the `config` target to other modules
add_library(Config::config ALIAS config)
