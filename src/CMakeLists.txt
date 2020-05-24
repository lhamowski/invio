set(source_files 
	core/audio_manager.cpp
	core/audio_manager.hpp
	support/assert.hpp
	support/com_interface.cpp 
	support/com_interface.hpp
	error.hpp)

add_library(invio-lib)
target_sources(invio-lib PRIVATE ${source_files})
target_include_directories(invio-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(invio-lib PRIVATE fmt::fmt)
source_group(TREE ${invio_SOURCE_DIR} FILES ${source_files})

add_executable(invio)
target_sources(invio PRIVATE main.cpp)
target_link_libraries(invio PRIVATE invio-lib)