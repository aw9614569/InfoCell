#pragma once

#include "util/Logging.h"

namespace infocell {

class UnitTester
{
public:
    UnitTester(Logger& logger);

    void start();

    Logger& logger;
};

} // namespace infocell