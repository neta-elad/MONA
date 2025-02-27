# Source files
set(GTA_SOURCES
    analyze.c analyze_acceptance.c basic.c copy.c dyn.c
    external.c gta.c makebasic.c minimize.c negation.c pairhash.c
    printgta.c product.c project.c projset.c reachable.c
    replace_indices.c restrict.c subsets.c types.c
)

# Header files
set(GTA_HEADERS
    gta.h dyn.h pairhash.h projset.h subsets.h
)

# Create the GTA static library
add_library(monagta STATIC ${GTA_SOURCES})

# Link dependencies (BDD and Mem)
target_link_libraries(monagta PUBLIC monabdd monamem)

# Include directories
target_include_directories(monagta PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Expose public headers
target_sources(monagta PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${GTA_HEADERS}
)
