set(lib_source_files 
	core/audio_manager.hpp
	core/audio_manager.cpp
	core/logger_manager.hpp
	core/logger_manager.cpp
	support/assert.hpp
	support/com_interface.hpp
	support/error.hpp
	support/error.cpp)

set(source_files 
	main.cpp 
	app.hpp 
	app.cpp
	config.hpp
	config.cpp)

add_library(invio-lib)
target_sources(invio-lib PRIVATE ${lib_source_files})
target_include_directories(invio-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(invio-lib PUBLIC 
	fmt::fmt
	kl::kl
	spdlog::spdlog)
source_group(TREE ${invio_SOURCE_DIR} FILES ${lib_source_files})

add_executable(invio)
target_sources(invio PRIVATE ${source_files})
target_link_libraries(invio PRIVATE 
	invio-lib
	kl::yaml)