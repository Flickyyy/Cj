# GC library CMakeLists.txt

set(GC_SOURCES
    garbage_collector.cpp
)

add_library(cj-gc STATIC ${GC_SOURCES})

target_include_directories(cj-gc PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cj-gc
    cj-types
)

# Enable all warnings
target_compile_options(cj-gc PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)