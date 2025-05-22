set(MONAFRONT_SOURCES
    ast.cpp astdump.cpp code.cpp codedump.cpp codesubst.cpp codetable.cpp
    freevars.cpp ident.cpp lib.cpp makeguide.cpp offsets.cpp predlib.cpp
    printline.cpp reduce.cpp signature.cpp st_dfa.cpp st_gta.cpp
    symboltable.cpp timer.cpp untyped.cpp
)

set(MONAFRONT_HEADERS
    ast.h code.h codetable.h deque.h env.h ident.h lib.h offsets.h predlib.h
    printline.h signature.h st_dfa.h st_gta.h str.h symboltable.h timer.h untyped.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(BISON_Parser_OUTPUTS parser.cpp)
set(FLEX_Scanner_OUTPUTS scanner.cpp)
list(APPEND MONAFRONT_SOURCES ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

add_library(monafront ${MONAFRONT_SOURCES} ${MONAFRONT_HEADERS})
target_include_directories(monafront PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(monafront
    PUBLIC monamem monabdd monadfa monagta Config::config
)
# Expose public headers
target_sources(monafront PUBLIC FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${MONA_HEADERS}
)
