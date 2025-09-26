# Source directory CMakeLists.txt for CJ language

# Add subdirectories for each component
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(ast)
add_subdirectory(ir)
add_subdirectory(vm)
add_subdirectory(gc)
add_subdirectory(jit)
add_subdirectory(types)
add_subdirectory(utils)

# Create main CJ library
set(CJ_CORE_SOURCES
    cj_engine.cpp
)

add_library(cj-core STATIC ${CJ_CORE_SOURCES})

target_link_libraries(cj-core
    cj-lexer
    cj-parser
    cj-ast
    cj-ir
    cj-vm
    cj-gc
    cj-jit
    cj-types
    cj-utils
)

target_include_directories(cj-core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Main executables
add_executable(cj-compiler
    compiler_main.cpp
)

add_executable(cj-vm-exe
    vm_main.cpp
)

add_executable(cj-repl
    repl_main.cpp
)

# Link executables with core library
target_link_libraries(cj-compiler cj-core)
target_link_libraries(cj-vm-exe cj-core)
target_link_libraries(cj-repl cj-core)

# Set output directories
set_target_properties(cj-compiler cj-vm-exe cj-repl
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS cj-core cj-compiler cj-vm-exe cj-repl
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)