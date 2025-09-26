# Types library CMakeLists.txt

set(TYPES_SOURCES
    value.cpp
)

add_library(cj-types STATIC ${TYPES_SOURCES})

target_include_directories(cj-types PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Enable all warnings
target_compile_options(cj-types PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)