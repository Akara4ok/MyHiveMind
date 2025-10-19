add_library(HiveMind
        HiveCommand.h HiveCommand.h
        HiveMind.cpp HiveMind.h
        RequestProcessor.cpp RequestProcessor.h
)

add_subdirectory(HiveEmulator)
add_subdirectory(Threads)

target_link_libraries(HiveMind PUBLIC Common WebServer HiveEmulator)

target_include_directories(HiveMind PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})