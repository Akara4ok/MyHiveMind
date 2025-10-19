//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_HIVEEMULATOR_H
#define MYHIVEMIND_HIVEEMULATOR_H

#include "IHiveController.h"

class HiveEmulator : public IHiveController {
public:
    HiveEmulator(std::unique_ptr<IHiveLogic> logic);

protected:
    void executeMove(double longitude, double latitude) override;
    void executeStop() override;
};

#endif //MYHIVEMIND_HIVEEMULATOR_H