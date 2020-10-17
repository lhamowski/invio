set(test_source_files 
	main.cpp 
	support/error_test.cpp)

add_executable(invio-test)
target_sources(invio-test PRIVATE ${test_source_files})
source_group(TREE ${invio_SOURCE_DIR} FILES ${test_source_files})

target_link_libraries(invio-test
    Catch2
    invio-lib)