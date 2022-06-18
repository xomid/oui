cmake_minimum_required (VERSION 3.8)

project (oui)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_EXAMPLES "Build the examples" ON)

add_subdirectory(oui)

if(BUILD_EXAMPLES)
	add_subdirectory (examples)
endif()

