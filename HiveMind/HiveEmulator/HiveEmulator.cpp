//
// Created by vlad on 10/18/25.
//

#include "HiveEmulator.h"

#include <utility>

HiveEmulator::HiveEmulator(std::unique_ptr<IHiveLogic> logic) : IHiveController(std::move(logic)) {
}

void HiveEmulator::executeMove(double longitude, double latitude) {
}

void HiveEmulator::executeStop() {
}
