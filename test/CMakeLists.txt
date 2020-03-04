add_executable(invio-test 
    main.cpp
    loopback_recorder_test.cpp)

target_link_libraries(invio-test
    invio-lib
    Catch
)