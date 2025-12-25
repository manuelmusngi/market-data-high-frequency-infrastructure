#include "mdp/time.hpp"
#include <thread>

namespace mdp::time {

TscClock& clock() {
    static TscClock c;
    static bool initialized = false;
    if (!initialized) {
        c.calibrate();
        initialized = true;
    }
    return c;
}

} // namespace mdp::time
