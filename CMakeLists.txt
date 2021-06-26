cmake_minimum_required(VERSION 3.16)
project(matchboxdb)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        matchboxdb
        src/main.cpp
        src/page.cpp
        src/page.h
        src/row_mapping_page.cpp
        src/row_mapping_page.h
        src/slotted_data_page.cpp
        src/slotted_data_page.h
        src/test_utils.h
        src/tuple.cpp
        src/tuple.h
        src/metadata_page.cpp
        src/metadata_page.h
        src/streamutils.h
        src/metadata.h
        src/streamutils.cpp
        src/page_cache.cpp
        src/page_cache.h
        src/table_initializer.cpp
        src/table_initializer.h
        src/metadata.cpp
        src/insert_object.cpp
        src/insert_object.h
        src/select_object.cpp
        src/select_object.h
        src/page_creator.cpp src/page_creator.h src/interpreter/lexer.cpp src/interpreter/lexer.h src/interpreter/token.h src/interpreter/token.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

ENABLE_TESTING()

add_executable(alltests tests/tests.cpp
        src/page.cpp
        src/page.h
        src/row_mapping_page.cpp
        src/row_mapping_page.h
        src/slotted_data_page.cpp
        src/slotted_data_page.h
        src/test_utils.h
        src/tuple.cpp
        src/tuple.h
        src/metadata_page.cpp
        src/metadata_page.h
        src/streamutils.h
        src/metadata.h
        src/streamutils.cpp
        src/page_cache.cpp
        src/page_cache.h
        src/table_initializer.cpp
        src/table_initializer.h
        src/metadata.cpp
        src/page_creator.cpp
        src/page_creator.h
        tests/stream_tests.cpp
        tests/table_initializer_tests.cpp
        tests/slotted_data_page_tests.cpp
        tests/page_creator_tests.cpp
        tests/page_cache_tests.cpp
        tests/test_utilities.cpp
        tests/test_utilities.h)
target_link_libraries(alltests gtest gmock gtest_main)

add_test(NAME test COMMAND alltests)

add_executable(interpreter_tests
        src/interpreter/lexer.cpp
        src/interpreter/lexer.h
        src/interpreter/token.h
        src/interpreter/token.cpp
        tests/interpreter_tests/lexer_tests.cpp src/interpreter/node.h src/interpreter/statement.h src/interpreter/expression.h src/interpreter/program.cpp src/interpreter/program.h src/interpreter/identifier.cpp src/interpreter/identifier.h src/interpreter/let_statement.cpp src/interpreter/let_statement.h src/interpreter/parser.cpp src/interpreter/parser.h tests/interpreter_tests/parser_tests.cpp src/interpreter/return_statement.cpp src/interpreter/return_statement.h src/interpreter/expression_statement.cpp src/interpreter/expression_statement.h src/interpreter/integer_literal.cpp src/interpreter/integer_literal.h src/interpreter/prefix_expression.cpp src/interpreter/prefix_expression.h src/interpreter/infix_expression.cpp src/interpreter/infix_expression.h src/interpreter/boolean_expression.cpp src/interpreter/boolean_expression.h src/interpreter/if_expression.cpp src/interpreter/if_expression.h src/interpreter/block_statement.cpp src/interpreter/block_statement.h src/interpreter/function_expression.cpp src/interpreter/function_expression.h src/interpreter/call_expression.cpp src/interpreter/call_expression.h)
target_link_libraries(interpreter_tests gtest gmock gtest_main)
add_test(NAME sqltest COMMAND interpreter_tests)


add_executable(repl src/interpreter/repl.cpp
        src/interpreter/lexer.cpp
        src/interpreter/lexer.h
        src/interpreter/token.h
        src/interpreter/token.cpp)