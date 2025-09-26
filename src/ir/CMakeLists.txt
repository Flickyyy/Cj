# IR library CMakeLists.txt

set(IR_SOURCES
    instruction.cpp
    ssa.cpp
)

add_library(cj-ir STATIC ${IR_SOURCES})

target_include_directories(cj-ir PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-ir
    cj-types
)

# Enable all warnings
target_compile_options(cj-ir PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)