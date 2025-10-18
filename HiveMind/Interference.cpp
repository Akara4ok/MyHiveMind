//
// Created by vlad on 10/18/25.
//

#include "Interference.h"

Interference Interference::fromJson(nlohmann::json &j) {
    Interference interference;
    interference.id = j.value("Id", "");
    interference.radiusKM = j.value("RadiusKM", 0);

    nlohmann::json location = j["Location"];
    interference.latitude = location.value("Latitude", 0);
    interference.longitude = location.value("Longitude", 0);
    return interference;
}
