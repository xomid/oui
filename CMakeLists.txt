cmake_minimum_required (VERSION 3.8)

project (oui)

set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")

add_subdirectory (oui)

add_subdirectory (examples)
