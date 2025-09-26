# Manual tests CMakeLists.txt

add_executable(manual_test_lexer manual_test_lexer.cpp)
add_executable(manual_test_parser manual_test_parser.cpp)

target_link_libraries(manual_test_lexer cj-core)
target_link_libraries(manual_test_parser cj-core)

set_target_properties(manual_test_lexer manual_test_parser
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)