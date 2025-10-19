//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_SIMPLEHIVELOGIC_H
#define MYHIVEMIND_SIMPLEHIVELOGIC_H

#include "IHiveLogic.h"

class SimpleHiveLogic : public IHiveLogic {
public:
    SimpleHiveLogic() = default;

    bool doMove(const HiveMindState &state, double latitude, double longitude) override;
    bool doStop() override;
    bool addInterference(const HiveMindState &state, Interference interference) override;
    bool removeInterference(const HiveMindState &state, std::string id) override;

private:
    int mInterferenceCount = 0;
};


#endif //MYHIVEMIND_SIMPLEHIVELOGIC_H