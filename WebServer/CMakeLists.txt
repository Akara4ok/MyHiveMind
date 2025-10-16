add_library(WebServer
        HttpConnection.cpp HttpConnection.h
        WebServer.cpp WebServer.h
)

target_link_libraries(WebServer PRIVATE Common)

target_include_directories(WebServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})