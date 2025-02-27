# Define the dfa2dot executable
add_executable(dfa2dot
    dfa2dot.c
    dfalib.c
)

# Define the gta2dot executable
add_executable(gta2dot
    gta2dot.c
    gtalib.c
)

# Ensure headers are available within this module
target_include_directories(dfa2dot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(gta2dot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
