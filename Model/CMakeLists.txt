set(MODEL_SOURCES
        model.cpp)

set(MODEL_HEADERS
        model.h)

# Create a static library (change to SHARED if needed)
add_library(monamodel STATIC ${MODEL_SOURCES})

# Specify include directories for use in other modules
target_include_directories(monamodel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Allow C++ code to link against this C library
target_compile_features(monamodel INTERFACE cxx_std_17)

# Expose public headers
target_sources(monamodel PUBLIC FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${MODEL_HEADERS}
)
