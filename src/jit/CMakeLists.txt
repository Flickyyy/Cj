# JIT library CMakeLists.txt

set(JIT_SOURCES
    jit_compiler.cpp
)

add_library(cj-jit STATIC ${JIT_SOURCES})

target_include_directories(cj-jit PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-jit
    cj-types
    cj-ir
)

# Enable all warnings
target_compile_options(cj-jit PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)