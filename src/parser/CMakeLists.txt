# Parser library CMakeLists.txt

set(PARSER_SOURCES
    parser.cpp
)

add_library(cj-parser STATIC ${PARSER_SOURCES})

target_include_directories(cj-parser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-parser
    cj-lexer
    cj-ast
    cj-types
)

# Enable all warnings
target_compile_options(cj-parser PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)