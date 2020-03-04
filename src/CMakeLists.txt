add_library(invio-lib)
target_sources(invio-lib PRIVATE 
	core/loopback_recorder.hpp core/loopback_recorder.cpp)

add_executable(invio)
target_sources(invio PRIVATE main.cpp)
target_link_libraries(invio PRIVATE invio-lib)