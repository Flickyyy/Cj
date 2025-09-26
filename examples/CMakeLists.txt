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

# Add working type system demo
add_executable(type_demo
    type_demo.cpp
)

# Add complete working demo
add_executable(complete_demo
    complete_demo.cpp
)

# Add SSA demonstration
add_executable(ssa_demo
    ssa_demo.cpp
)

# Add optimization improvements demo
add_executable(optimization_demo
    optimization_demo.cpp
)

# Link with core library
target_link_libraries(hello_world cj-core)
target_link_libraries(lexer_demo cj-core)
target_link_libraries(parser_demo cj-core)
target_link_libraries(type_demo cj-types cj-semantic)
target_link_libraries(complete_demo cj-types cj-semantic)
target_link_libraries(ssa_demo cj-ir cj-jit cj-types)
target_link_libraries(optimization_demo cj-ir cj-jit cj-types)

# Set output directory
set_target_properties(hello_world lexer_demo parser_demo type_demo complete_demo ssa_demo optimization_demo
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