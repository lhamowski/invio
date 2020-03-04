set(source_files 
	core/loopback_recorder.cpp
	core/loopback_recorder.hpp) 

add_library(invio-lib)
target_sources(invio-lib PRIVATE ${source_files})
source_group(TREE ${invio_SOURCE_DIR} FILES ${source_files})

add_executable(invio)
target_sources(invio PRIVATE main.cpp)
target_link_libraries(invio PRIVATE invio-lib)