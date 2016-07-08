include_directories("../include")

add_executable(array array.cpp)
target_link_libraries(utility)
target_compile_features(array PRIVATE cxx_range_for)

add_executable(md_vector md_vector.cpp)
target_link_libraries(utility)
target_compile_features(md_vector PRIVATE cxx_range_for)

enable_testing()
add_test(array array)
add_test(md_vector md_vector)
