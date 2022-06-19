cmake_minimum_required (VERSION 3.8)

project (oui)

if(WIN32)
	set(GLOBAL_OUT_DIR "C:/Users/Omid/Documents/cmake/out")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GLOBAL_OUT_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set(LIBRARY_OUTPUT_PATH "${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	
	link_directories("${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	include_directories(${PROJECT_NAME} "${GLOBAL_OUT_DIR}/include/ocom")
endif()

add_subdirectory(oui)

option(BUILD_EXAMPLES "Build the examples" OFF)
if(BUILD_EXAMPLES)
	add_subdirectory (examples)
endif()

