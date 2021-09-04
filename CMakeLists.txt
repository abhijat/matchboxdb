cmake_minimum_required(VERSION 3.16)
project(matchboxdb)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

ENABLE_TESTING()

add_executable(
        alltests
        src/command_executor.cpp
        src/command_executor.h
        src/insert_object.cpp
        src/insert_object.h
        src/metadata.cpp
        src/metadata.h
        src/metadata_page.cpp
        src/metadata_page.h
        src/page.cpp
        src/page.h
        src/page_cache.cpp
        src/page_cache.h
        src/page_creator.cpp
        src/page_creator.h
        src/page_scanner.cpp
        src/page_scanner.h
        src/page_visitors.cpp
        src/page_visitors.h
        src/row_mapping_page.cpp
        src/row_mapping_page.h
        src/slotted_data_page.cpp
        src/slotted_data_page.h
        src/sql_parser/boolean_literal.cpp
        src/sql_parser/boolean_literal.h
        src/sql_parser/create_statement.cpp
        src/sql_parser/create_statement.h
        src/sql_parser/delete_statement.cpp
        src/sql_parser/delete_statement.h
        src/sql_parser/expression.h
        src/sql_parser/expression_statement.cpp
        src/sql_parser/expression_statement.h
        src/sql_parser/field_definition.cpp
        src/sql_parser/field_definition.h
        src/sql_parser/identifier.cpp
        src/sql_parser/identifier.h
        src/sql_parser/infix_expression.cpp
        src/sql_parser/infix_expression.h
        src/sql_parser/insert_statement.cpp
        src/sql_parser/insert_statement.h
        src/sql_parser/integer_literal.cpp
        src/sql_parser/integer_literal.h
        src/sql_parser/lexer.cpp
        src/sql_parser/lexer.h
        src/sql_parser/node.cpp
        src/sql_parser/node.h
        src/sql_parser/parser.cpp
        src/sql_parser/parser.h
        src/sql_parser/select_statement.cpp
        src/sql_parser/select_statement.h
        src/sql_parser/statement_visitor.h
        src/sql_parser/statement_visitor.h
        src/sql_parser/string_literal.cpp
        src/sql_parser/string_literal.h
        src/sql_parser/table.cpp
        src/sql_parser/table.h
        src/sql_parser/token.cpp
        src/sql_parser/token.h
        src/sql_parser/update_statement.cpp
        src/sql_parser/update_statement.h
        src/streamutils.cpp
        src/streamutils.h
        src/table_initializer.cpp
        src/table_initializer.h
        src/test_utils.h
        src/tuple.cpp
        src/tuple.h
        tests/insert_statement_tests.cpp
        tests/page_cache_tests.cpp
        tests/page_creator_tests.cpp
        tests/page_scanner_tests.cpp
        tests/slotted_data_page_tests.cpp
        tests/sql_parser_tests/lexer_tests.cpp
        tests/sql_parser_tests/lexer_tests.cpp
        tests/sql_parser_tests/parser_tests.cpp
        tests/stream_tests.cpp
        tests/table_initializer_tests.cpp
        tests/test_utilities.cpp
        tests/test_utilities.h
        tests/tests.cpp
        src/sql_parser/expression_visitor.cpp src/sql_parser/expression_visitor.h tests/sql_parser_tests/evaluator_tests.cpp src/actions/select_action.cpp src/actions/select_action.h)
target_link_libraries(alltests gtest gmock gtest_main)

add_test(NAME test COMMAND alltests)


add_executable(
        repl
        src/command_executor.cpp
        src/command_executor.h
        src/metadata.cpp
        src/metadata.h
        src/repl.cpp
        src/sql_parser/boolean_literal.cpp
        src/sql_parser/boolean_literal.h
        src/sql_parser/create_statement.cpp
        src/sql_parser/create_statement.h
        src/sql_parser/delete_statement.cpp
        src/sql_parser/delete_statement.h
        src/sql_parser/expression.h
        src/sql_parser/expression_statement.cpp
        src/sql_parser/expression_statement.h
        src/sql_parser/field_definition.cpp
        src/sql_parser/field_definition.h
        src/sql_parser/identifier.cpp
        src/sql_parser/identifier.h
        src/sql_parser/infix_expression.cpp
        src/sql_parser/infix_expression.h
        src/sql_parser/insert_statement.cpp
        src/sql_parser/insert_statement.h
        src/sql_parser/integer_literal.cpp
        src/sql_parser/integer_literal.h
        src/sql_parser/lexer.cpp
        src/sql_parser/lexer.h
        src/sql_parser/node.cpp
        src/sql_parser/node.h
        src/sql_parser/parser.cpp
        src/sql_parser/parser.h
        src/sql_parser/select_statement.cpp
        src/sql_parser/select_statement.h
        src/sql_parser/statement.h
        src/sql_parser/statement_visitor.h
        src/sql_parser/string_literal.cpp
        src/sql_parser/string_literal.h
        src/sql_parser/table.cpp
        src/sql_parser/table.h
        src/sql_parser/token.cpp
        src/sql_parser/token.h
        src/sql_parser/update_statement.cpp
        src/sql_parser/update_statement.h
        src/sql_parser/expression_visitor.cpp src/sql_parser/expression_visitor.h src/actions/select_action.cpp src/actions/select_action.h)
