function(add_dependencies file_name)
endfunction()

function(compile_cpp_file file_path)
	get_filename_component(file_name "${file_path}" NAME_WE)
	message(STATUS "Generate build config: ${file_path} ---> ${file_name}")
	add_library(${file_name} STATIC "${file_path}")
	add_dependencies(${file_name})
endfunction()

file(GLOB_RECURSE all_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
foreach(cpp_file ${all_files})
	compile_cpp_file(${cpp_file})
endforeach()
