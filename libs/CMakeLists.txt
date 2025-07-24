function(add_dependencies file_name)
	# packages
	find_package(fmt CONFIG REQUIRED)

	# links
	target_link_libraries(${file_name} PRIVATE fmt::fmt)
endfunction()

function(configure_property file_name)
	set_property(TARGET ${file_name} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endfunction()

function(compile_cpp_file file_path)
	get_filename_component(file_name "${file_path}" NAME_WE)

	message(STATUS "Generate build config: ${file_path} ---> ${file_name}")

	add_library(${file_name} STATIC "${file_path}")

	set_target_properties(${file_name} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES)

	add_dependencies(${file_name})

	configure_property(${file_name})
endfunction()

file(GLOB_RECURSE all_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
foreach(cpp_file ${all_files})
	compile_cpp_file(${cpp_file})
endforeach()
