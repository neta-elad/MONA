set(BDD_SOURCES
    bdd.c bdd_cache.c bdd_double.c bdd_dump.c
    bdd_external.c bdd_manager.c bdd_trace.c
    hash.c
)

# Header files
set(BDD_HEADERS
    bdd.h bdd_dump.h bdd_external.h bdd_internal.h hash.h
)

# Create the BDD library
add_library(monabdd STATIC ${BDD_SOURCES})

# Link against Mem module (dependency)
target_link_libraries(monabdd PUBLIC monamem)

# Include directories
target_include_directories(monabdd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Expose public headers
target_sources(monabdd PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${BDD_HEADERS}
)
