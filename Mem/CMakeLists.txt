set(MEM_SOURCES
    dlmalloc.c mem.c)

set(MEM_HEADERS
    dlmalloc.h mem.h)

# Create a static library (change to SHARED if needed)
add_library(monamem STATIC ${MEM_SOURCES})

# Specify include directories for use in other modules
target_include_directories(monamem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Expose public headers
target_sources(monamem PUBLIC FILE_SET HEADERS 
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} 
    FILES ${MEM_HEADERS}
)
