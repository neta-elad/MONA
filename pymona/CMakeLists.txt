set(VENV_PATH "${CMAKE_BINARY_DIR}/.venv")
find_package(Python 3.12 COMPONENTS Interpreter Development.Module REQUIRED)
execute_process(COMMAND "${Python_EXECUTABLE}" -m venv "${VENV_PATH}")

# Update the environment with VIRTUAL_ENV variable (mimic the activate script)
set(ENV{VIRTUAL_ENV} "${VENV_PATH}")
# Change the context of the search
set(Python_FIND_VIRTUALENV FIRST)
# Unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
unset(Python_EXECUTABLE)
# Launch a new search
find_package(Python 3.12 COMPONENTS Interpreter Development.Module REQUIRED)

# Install nanobind
execute_process(
        COMMAND "${Python_EXECUTABLE}" -m pip install nanobind
)
execute_process(
        COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(pymona pymona.cpp)
target_link_libraries(pymona PRIVATE mona)
