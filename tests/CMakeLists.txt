# Tests CMakeLists.txt for CJ language

# Only build tests if googletest is available
if(TARGET gtest)
    # Test sources
    set(TEST_SOURCES
        test_main.cpp
        test_lexer.cpp
        test_parser.cpp
        test_value.cpp
        test_vm.cpp
    )

    # Create test executable
    add_executable(cj-tests ${TEST_SOURCES})

    # Link with core library and gtest
    target_link_libraries(cj-tests
        cj-core
        gtest
        gtest_main
    )

    target_include_directories(cj-tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    # Register tests with CTest
    add_test(NAME CJTests COMMAND cj-tests)

    # Set output directory
    set_target_properties(cj-tests
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Add manual test programs
add_subdirectory(manual)