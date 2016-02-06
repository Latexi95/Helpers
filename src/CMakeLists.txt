file(GLOB SRC "*.h" "*.cpp" "../include/*.h")

include_directories("../include")

add_library(utility ${SRC})
target_compile_features(utility PRIVATE cxx_range_for)
