add_library(WebServer
        HttpConnection.cpp HttpConnection.h
        HttpRequest.cpp HttpRequest.h
        HttpResponse.cpp HttpResponse.h
        HttpServer.cpp HttpServer.h
        HttpClient.cpp HttpClient.h
        HttpParser.cpp
        HttpParser.h
)

target_link_libraries(WebServer PRIVATE Common)

target_include_directories(WebServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})