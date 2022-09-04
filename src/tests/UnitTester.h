#pragma once

#include "util/Logging.h"

namespace synth {

class UnitTester
{
public:
    UnitTester(Logger& logger);

    void start();

    Logger& logger;
};

} // namespace synth