set(MONA_SOURCES
    ast.cpp astdump.cpp code.cpp codedump.cpp codesubst.cpp codetable.cpp
    freevars.cpp ident.cpp lib.cpp makeguide.cpp offsets.cpp predlib.cpp
    printline.cpp reduce.cpp signature.cpp st_dfa.cpp st_gta.cpp
    symboltable.cpp timer.cpp untyped.cpp
)

set(MONA_HEADERS
    ast.h code.h codetable.h deque.h env.h ident.h lib.h offsets.h predlib.h
    printline.h signature.h st_dfa.h st_gta.h str.h symboltable.h timer.h untyped.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
BISON_TARGET(Parser parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
FLEX_TARGET(Scanner scanner.lpp ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)
list(APPEND MONA_SOURCES ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

add_library(mona ${MONA_SOURCES} ${MONA_HEADERS})
target_include_directories(mona PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mona 
    PUBLIC monamem monabdd monadfa monagta Config::config
)
# Expose public headers
target_sources(mona PUBLIC FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${MONA_HEADERS}
)

add_executable(mona_bin mona.cpp)
set_target_properties(mona_bin PROPERTIES OUTPUT_NAME "mona")
target_include_directories(mona_bin PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mona_bin PRIVATE mona)
