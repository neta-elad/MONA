# Source files
set(DFA_SOURCES
    analyze.c basic.c dfa.c external.c makebasic.c
    minimize.c prefix.c printdfa.c product.c project.c quotient.c
)

# Header files
set(DFA_HEADERS
    dfa.h hash.h
)

# Create the DFA static library
add_library(monadfa STATIC ${DFA_SOURCES})

# Link dependencies (BDD and Mem)
target_link_libraries(monadfa PUBLIC monabdd monamem)

# Include directories
target_include_directories(monadfa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Expose public headers
target_sources(monadfa PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${DFA_HEADERS}
)
