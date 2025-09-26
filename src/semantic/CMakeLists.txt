# Semantic analysis library CMakeLists.txt

set(SEMANTIC_SOURCES
    type_checker.cpp
)

add_library(cj-semantic STATIC ${SEMANTIC_SOURCES})

target_include_directories(cj-semantic PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-semantic PRIVATE
    cj-types
)

# Enable all warnings
target_compile_options(cj-semantic PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)