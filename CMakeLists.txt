cmake_minimum_required(VERSION 4.0)
project(MyHiveMind)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(MyHiveMind main.cpp)

add_subdirectory(Tests)
add_subdirectory(Common)
add_subdirectory(WebServer)

target_link_libraries(MyHiveMind PRIVATE Common WebServer)

