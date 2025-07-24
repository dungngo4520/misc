function(add_dependencies file_name)
	# packages
	find_package(absl CONFIG REQUIRED)
	find_package(CLI11 CONFIG REQUIRED)
	find_package(flatbuffers CONFIG REQUIRED)
	find_package(fmt CONFIG REQUIRED)
	find_package(GTest CONFIG REQUIRED)
	find_package(Libevent CONFIG REQUIRED)
	find_package(spdlog CONFIG REQUIRED)

	# links
	target_link_libraries(${file_name} PRIVATE absl::any absl::log absl::base absl::bits)
	target_link_libraries(${file_name} PRIVATE CLI11::CLI11)
	target_link_libraries(${file_name} PRIVATE flatbuffers::flatbuffers)
	target_link_libraries(${file_name} PRIVATE fmt::fmt)
	target_link_libraries(${file_name} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
	target_link_libraries(${file_name} PRIVATE libevent::core libevent::extra)
	target_link_libraries(${file_name} PRIVATE spdlog::spdlog)
endfunction()

function(configure_property file_name)
	set_property(TARGET ${file_name} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endfunction()

function(compile_cpp_file file_path)
	get_filename_component(file_name "${file_path}" NAME_WE)

	message(STATUS "Compiling ${file_path} ---> ${file_name}")

	add_executable(${file_name} "${file_path}")

	set_target_properties(${file_name} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES)

	add_dependencies(${file_name})

	configure_property(${file_name})
endfunction()

file(GLOB_RECURSE all_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
foreach(cpp_file ${all_files})
	compile_cpp_file(${cpp_file})
endforeach()
