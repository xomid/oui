cmake_minimum_required(VERSION 3.8)

project(oui_sandbox) 

add_compile_definitions(
	OUI_TEST_APP
)

set(SUBSYSTEM WIN32)

if(WIN32)
	set(GLOBAL_OUT_DIR "C:/Users/Omid/Documents/cmake/out")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GLOBAL_OUT_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set(LIBRARY_OUTPUT_PATH "${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	
	link_directories("${GLOBAL_OUT_DIR}/lib/${CMAKE_BUILD_TYPE}")
	include_directories(${PROJECT_NAME} "${GLOBAL_OUT_DIR}/include/ocom" "../oui/")
endif()

include_directories("../include")

file(GLOB SRC "*.cpp")

file(GLOB _OUI_SRC "../src/*.cpp")

add_executable(oui_sandbox ${SUBSYSTEM} ${SRC} ${_OUI_SRC})

target_link_libraries(oui_sandbox ocom)

set_property(TARGET oui_sandbox PROPERTY CXX_STANDARD 17)

target_compile_options(oui_sandbox PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W0>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w>
)
