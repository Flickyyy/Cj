# VM library CMakeLists.txt

set(VM_SOURCES
    virtual_machine.cpp
)

add_library(cj-vm STATIC ${VM_SOURCES})

target_include_directories(cj-vm PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-vm
    cj-types
    cj-ir
    cj-gc
    cj-jit
)

# Enable all warnings
target_compile_options(cj-vm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)