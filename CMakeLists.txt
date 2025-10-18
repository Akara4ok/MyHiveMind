cmake_minimum_required(VERSION 4.0)
project(MyHiveMind)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(MyHiveMind main.cpp)

add_subdirectory(Tests)
add_subdirectory(Common)
add_subdirectory(HiveMind)
add_subdirectory(WebServer)

add_custom_command(TARGET MyHiveMind POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/appsettings.json
        $<TARGET_FILE_DIR:MyHiveMind>/appsettings.json
)

target_link_libraries(MyHiveMind PRIVATE Common WebServer HiveMind)

