add_library(Common
        HttpRequest.cpp HttpRequest.h
        HttpResponse.cpp HttpResponse.h
        SafeQueue.hpp
)

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})