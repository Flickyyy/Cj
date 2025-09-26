# Lexer library CMakeLists.txt

set(LEXER_SOURCES
    token.cpp
    lexer.cpp
)

add_library(cj-lexer STATIC ${LEXER_SOURCES})

target_include_directories(cj-lexer PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-lexer
    cj-types
)

# Enable all warnings
target_compile_options(cj-lexer PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)