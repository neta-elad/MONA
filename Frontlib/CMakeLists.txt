set(MONA_SOURCES
    model.cpp utils.cpp
)

set(MONA_HEADERS
    model.h utils.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(mona ${MONA_SOURCES} ${MONA_HEADERS})
target_include_directories(mona PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mona 
    PUBLIC monafront Config::config
)
# Expose public headers
target_sources(mona PUBLIC FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${MONA_HEADERS}
)
