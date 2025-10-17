//
// Created by vlad on 10/17/25.
//

#include "Task.h"

Task::TaskType Task::intToType(int value) {
    switch (value) {
        case 0: return None;
        case 1: return Stop;
        case 2: return Move;
        case 3: return GetTelemetry;
        case 4: return SetState;
        case 5: return AddInterference;
        case 6: return RemoveInterference;
        case 7: return SetError;
        default: return None;
    }
    return None;
}
