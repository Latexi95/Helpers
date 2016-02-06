include_directories("../include")

add_executable(array array.cpp)
target_link_libraries(utility)
target_compile_features(array PRIVATE cxx_range_for)

enable_testing()
add_test(array array)
