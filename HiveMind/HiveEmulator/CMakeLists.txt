add_library(HiveEmulator
        HiveEmulator.cpp HiveEmulator.h
        HiveMindState.cpp HiveMindState.h
        IHiveController.cpp IHiveController.h
        IHiveLogic.cpp IHiveLogic.h
        Interference.cpp Interference.h
        SimpleHiveLogic.cpp SimpleHiveLogic.h
)

target_link_libraries(HiveEmulator PRIVATE Common)

target_include_directories(HiveEmulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})