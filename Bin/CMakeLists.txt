add_executable(mona_bin mona.cpp)
set_target_properties(mona_bin PROPERTIES OUTPUT_NAME "mona")
target_include_directories(mona_bin PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mona_bin PRIVATE monafront)