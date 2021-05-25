cmake_minimum_required(VERSION 3.16)
project(matchboxdb)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        matchboxdb
        src/main.cpp
        src/file_write.h
        src/file_write.cpp
        src/file_read.h
        src/file_read.cpp
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
)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test tests/tests.cpp
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
        )
target_link_libraries(test gtest gmock gtest_main)
