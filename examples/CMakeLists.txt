cmake_minimum_required(VERSION 3.8)

add_executable(Simple main.cpp container.cpp)

target_link_libraries(Simple oui)

include_directories(Simple ../oui)