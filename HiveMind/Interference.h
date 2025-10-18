//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_INTERFERENCE_H
#define MYHIVEMIND_INTERFERENCE_H

#include <string>
#include <nlohmann/json.hpp>

struct Interference {
    static Interference fromJson(nlohmann::json& j);

    std::string id;
    double radiusKM = 0;
    double longitude = 0;
    double latitude = 0;
};


#endif //MYHIVEMIND_INTERFERENCE_H