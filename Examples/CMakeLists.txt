# List of example executables
add_executable(bdd_example bdd_example.c)
add_executable(gta_example gta_example.c)
add_executable(presburger_transduction presburger_transduction.c)
add_executable(presburger_analysis presburger_analysis.c)

# Link dependencies
target_link_libraries(bdd_example PRIVATE monabdd)
target_link_libraries(gta_example PRIVATE monagta)
target_link_libraries(presburger_transduction PRIVATE monadfa)
target_link_libraries(presburger_analysis PRIVATE monadfa)

# Extra files (handled manually)
set(EXTRA_DIST
    bdd_volatility ab1.mona even_with_pred.mona minusmodulo.mona
    regexp.mona ab2.mona html.mona nadder.mona even.mona hyman.mona
    plusmodulo.mona even_with_assert.mona lossy_queue.mona presburger.mona
)

# Custom target to copy extra files to the build directory
add_custom_target(copy_extra_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${EXTRA_DIST} ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
