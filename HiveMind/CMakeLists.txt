add_library(HiveMind
        HiveCommand.h HiveCommand.h
        HiveEmulator.cpp HiveEmulator.h
        HiveMind.cpp HiveMind.h
        HiveMindState.cpp HiveMindState.h
        IHiveLogic.cpp IHiveLogic.h
        RequestProcessor.cpp RequestProcessor.h
        SimpleHiveLogic.cpp SimpleHiveLogic.h
        Interference.cpp
        Interference.h
)

target_link_libraries(HiveMind PRIVATE Common WebServer)

target_include_directories(HiveMind PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})