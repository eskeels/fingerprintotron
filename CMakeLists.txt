project (fingerprinter)

add_compile_options(-std=c++11 -fPIC)

enable_testing()

if (COVERAGE)
    MESSAGE(STATUS "Enabling coverage")
    #SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
    #SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    #SET(CMAKE_EXE_LINKER_FLAGS " --coverage -fprofile-arcs -ftest-coverage")a
    add_definitions(--coverage -O0 -fno-exceptions -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    set(LINKER_FLAGS "${LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "--coverage")
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
endif(COVERAGE)

add_executable (fingerprinterutil murmur3.c main.cpp)
target_link_libraries(fingerprinterutil icudata icuio icuuc)

add_library (fingerprinter SHARED murmur3.c)

add_subdirectory(unittests)
