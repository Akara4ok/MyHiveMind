//
// Created by vlad on 10/18/25.
//

#include "HiveEmulator.h"

#include <iostream>
#include <utility>

namespace {
    constexpr double R = 6371;

    constexpr double DEG2RAD = M_PI / 180.0;
    constexpr double RAD2DEG = 180.0 / M_PI;

    double deg2rad(double deg) { return deg * DEG2RAD; }
    double rad2deg(double rad) { return rad * RAD2DEG; }
}

HiveEmulator::HiveEmulator(std::unique_ptr<IHiveLogic> logic) : IHiveController(std::move(logic)) {
}

void HiveEmulator::executeMove(double longitude, double latitude) {
    while (distance(mState.latitude, mState.longitude, latitude, longitude) > 0.1) {
        doSingleMove(100, longitude, latitude);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (mAbortPrevTasks) {
            mAbortPrevTasks = false;
            return;
        }
    }
}

void HiveEmulator::executeStop() {}

void HiveEmulator::doSingleMove(double ms, double longitude, double latitude) {
    double phi1 = deg2rad(mState.latitude);
    double lambda1 = deg2rad(mState.longitude);
    double phi2 = deg2rad(latitude);
    double lambda2 = deg2rad(longitude);

    double dLambda = lambda2 - lambda1;

    double azimuth = atan2(sin(dLambda) * cos(phi2),
                           cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(dLambda));

    double d = mState.speed * (ms / 1000.0);

    double newPhi = asin(sin(phi1) * cos(d / R) + cos(phi1) * sin(d / R) * cos(azimuth));
    double newLambda = lambda1 + atan2(sin(azimuth) * sin(d / R) * cos(phi1),
                                       cos(d / R) - sin(phi1) * sin(newPhi));

    newLambda = fmod((newLambda + 3*M_PI), (2*M_PI)) - M_PI;

    {
        std::lock_guard lock(mMutex);
        mState.latitude = rad2deg(newPhi);
        mState.longitude = rad2deg(newLambda);
    }
}

double HiveEmulator::distance(double lat1, double lon1, double lat2, double lon2) const {
    double phi1 = lat1 * DEG2RAD;
    double phi2 = lat2 * DEG2RAD;
    double dphi = (lat2 - lat1) * DEG2RAD;
    double dlambda = (lon2 - lon1) * DEG2RAD;

    double a = sin(dphi / 2) * sin(dphi / 2) +
               cos(phi1) * cos(phi2) * sin(dlambda / 2) * sin(dlambda / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c * 1000;
}
