project (unittests)
include_directories(..)
add_compile_options(-std=c++11)

enable_testing()
add_executable (test_ngramiterator test_ngramiterator.cpp)
target_link_libraries(test_ngramiterator icudata icuio icuuc)
add_test (test_ngramiterator test_ngramiterator)

add_executable (test_hasher test_hasher.cpp ../murmur3.c)
target_link_libraries(test_hasher icudata icuio icuuc)
add_test(test_hasher test_hasher)

add_executable (test_fingerprintgenerator test_fingerprintgenerator.cpp ../murmur3.c)
target_link_libraries(test_fingerprintgenerator icudata icuio icuuc)
add_test (test_fingerprintgenerator test_fingerprintgenerator)

add_executable (test_document test_document.cpp)
target_link_libraries(test_document)
add_test (test_document test_document)

add_executable (test_comparisonresult test_comparisonresult.cpp)
target_link_libraries(test_comparisonresult)
add_test (test_comparisonresult test_comparisonresult)

add_executable (test_system test_system.cpp ../murmur3.c)
target_link_libraries(test_system icudata icuio icuuc)
add_test (test_system test_system)

