cmake_minimum_required(VERSION 3.8)

set(APPNAME "Simple")

include_directories(${APPNAME} ../oui)

add_executable(${APPNAME} main.cpp container.cpp)

target_link_libraries(${APPNAME} oui)
