include(FetchContent)
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(catch2)

add_executable(MyTests
        test_main.cpp
        test_queue.cpp
        test_response.cpp
)

target_link_libraries(MyTests PRIVATE Common WebServer Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(MyTests)