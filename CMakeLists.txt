project (fingerprinter)

add_compile_options(-std=c++11 -fPIC)

enable_testing()

if (COVERAGE)
    message(STATUS "Enabling coverage")
    set(CMAKE_CXX_FLAGS "--coverage -O0 -fno-exceptions -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_SHARED_LINKER_FLAGS "--coverage")
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
endif(COVERAGE)

add_library (fingerprinter SHARED murmur3.c)

add_executable (compare main.cpp)
target_link_libraries(compare fingerprinter icudata icuio icuuc)


add_subdirectory(unittests)
