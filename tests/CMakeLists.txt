## test_mem
add_executable(test_mem test_mem.c)

# Link against Mem
target_link_libraries(test_mem PRIVATE monamem)

# Enable C++17
target_compile_features(test_mem PRIVATE cxx_std_17)

## test_bdd
add_executable(test_bdd test_bdd.c)
target_link_libraries(test_bdd PRIVATE monabdd)
target_compile_features(test_bdd PRIVATE cxx_std_17)

## test_dfa
add_executable(test_dfa test_dfa.c)
target_link_libraries(test_dfa PRIVATE monadfa)
target_compile_features(test_dfa PRIVATE cxx_std_17)

## test_gta
add_executable(test_gta test_gta.c)
target_link_libraries(test_gta PRIVATE monagta)
target_compile_features(test_gta PRIVATE cxx_std_17)

# test_libmona
add_executable(test_libmona test_libmona.cpp)
target_link_libraries(test_libmona PRIVATE mona monamodel)
target_compile_features(test_libmona PRIVATE cxx_std_17)