## test_mem
add_executable(test_mem test_mem.c)

# Link against Mem
target_link_libraries(test_mem PRIVATE monamem)

## test_bdd
add_executable(test_bdd test_bdd.c)
target_link_libraries(test_bdd PRIVATE monabdd)

## test_dfa
add_executable(test_dfa test_dfa.c)
target_link_libraries(test_dfa PRIVATE monadfa)

## test_gta
add_executable(test_gta test_gta.c)
target_link_libraries(test_gta PRIVATE monagta)

# test_libmona
add_executable(test_libmona test_libmona.cpp)
target_link_libraries(test_libmona PRIVATE mona)

# test_ptrs
add_executable(test_ptrs test_ptrs.cpp)
