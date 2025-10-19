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

private:
    void doSingleMove(double ms, double longitude, double latitude);
    double distance(double lat1, double lon1, double lat2, double lon2) const;
};

#endif //MYHIVEMIND_HIVEEMULATOR_H