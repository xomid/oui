cmake_minimum_required (VERSION 3.8)

if(WIN32)
	set(CMAKE_PREFIX_PATH "C:/dev/vcpkg/installed/x64-windows/share")
	set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

project (oui)

add_compile_definitions(
	_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
	DEBUG_UPDATEFRAME
)

set(LIBTYPE SHARED)

if(WIN32)
	set(GLOBAL_OUT_DIR "C:/Users/Omid/Documents/cmake/out")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GLOBAL_OUT_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set(LIBRARY_OUTPUT_PATH "${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")

	link_directories("${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	include_directories(${PROJECT_NAME} "${GLOBAL_OUT_DIR}/include/ocom")
endif()

find_package(ZLIB REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(expat CONFIG REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${HARFBUZZ_INCLUDE_DIRS})

include_directories("include")

file(GLOB SRC "src/*.cpp")

add_library(${PROJECT_NAME} ${LIBTYPE} ${SRC})

target_link_libraries(${PROJECT_NAME} ocom freetype harfbuzz expat::expat)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

if(WIN32)
	set(PUBLIC_HEADER_LOCATION "${GLOBAL_OUT_DIR}/include/${PROJECT_NAME}")
	file(GLOB PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
	add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD 
		COMMAND ${CMAKE_COMMAND} -E make_directory ${PUBLIC_HEADER_LOCATION})
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${PUBLIC_HEADERS} ${PUBLIC_HEADER_LOCATION}
	)
endif()

option(BUILD_EXAMPLES "Build the examples" OFF)
if(BUILD_EXAMPLES)
	add_subdirectory (examples)
endif()

