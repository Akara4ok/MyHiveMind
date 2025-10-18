//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_HIVEMINDSTATE_H
#define MYHIVEMIND_HIVEMINDSTATE_H


struct HiveMindState {
    enum State {
        Error,
        Stop,
        Move,
    };

    State state = State::Error;
    double longitude = 0;
    double latitude = 0;
    double height = 0;
    double speed = 0;
};


#endif //MYHIVEMIND_HIVEMINDSTATE_H