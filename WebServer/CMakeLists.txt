add_library(WebServer
        HttpClient.cpp HttpClient.h
        HttpConnection.cpp HttpConnection.h
        HttpParser.cpp HttpParser.h
        HttpRequest.cpp HttpRequest.h
        HttpResponse.cpp HttpResponse.h
        HttpServer.cpp HttpServer.h
        HttpClientWorker.cpp
        HttpClientWorker.h
)

target_link_libraries(WebServer PRIVATE Common)

target_include_directories(WebServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})