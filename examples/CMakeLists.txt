# Examples CMakeLists.txt for CJ language

# Create example programs
add_executable(hello_world
    hello_world.cpp
)

add_executable(lexer_demo
    lexer_demo.cpp
)

add_executable(parser_demo  
    parser_demo.cpp
)

# Add static typing demo
add_executable(static_typing_demo
    static_typing_demo/demo.cpp
)

# Link with core library
target_link_libraries(hello_world cj-core)
target_link_libraries(lexer_demo cj-core)
target_link_libraries(parser_demo cj-core)
target_link_libraries(static_typing_demo cj-types)

# Set output directory
set_target_properties(hello_world lexer_demo parser_demo static_typing_demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)

# Copy example CJ source files
file(COPY
    hello.cj
    fibonacci.cj
    calculator.cj
    DESTINATION ${CMAKE_BINARY_DIR}/examples
)