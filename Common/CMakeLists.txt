add_library(Common
        Task.cpp Task.h
        SafeQueue.hpp
)

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})