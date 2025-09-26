# AST library CMakeLists.txt

set(AST_SOURCES
    ast_node.cpp
    ast_visitor.cpp
)

add_library(cj-ast STATIC ${AST_SOURCES})

target_include_directories(cj-ast PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-ast
    cj-types
)

# Enable all warnings
target_compile_options(cj-ast PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)