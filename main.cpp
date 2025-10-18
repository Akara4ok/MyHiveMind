#include <iostream>

#include "HiveMind.h"

int main() {
    HiveMind hiveMind("appsettings.json");
    hiveMind.start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    hiveMind.stop();

    return 0;
}
