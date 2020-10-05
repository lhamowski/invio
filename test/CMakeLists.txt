add_executable(invio-test 
    main.cpp
)

target_link_libraries(invio-test
    Catch2
    invio-lib
)