cmake_minimum_required (VERSION 3.8)

project (oui)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(oui ocom/ocom)

add_subdirectory(ocom)

add_subdirectory(oui)

option(BUILD_EXAMPLES "Build the examples" OFF)
if(BUILD_EXAMPLES)
	add_subdirectory (examples)
endif()

