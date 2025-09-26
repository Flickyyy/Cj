# Utils library CMakeLists.txt

set(UTILS_SOURCES
    string_utils.cpp
    file_utils.cpp
)

add_library(cj-utils STATIC ${UTILS_SOURCES})

target_include_directories(cj-utils PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Enable all warnings
target_compile_options(cj-utils PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)