# CMakeList.txt : CMake project for src, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (src "src.cpp" "src.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET src PROPERTY CXX_STANDARD 20)
endif()
