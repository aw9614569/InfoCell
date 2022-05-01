#pragma once

#include <nlohmann/json.hpp>
#include "cells/Cells.h"

namespace synth {

class ArcDemonstration
{
public:
    cells::Sensor m_input;
    cells::Sensor m_output;
};

class ArcTask
{
public:
    std::vector<ArcDemonstration> m_demonstrations;
    cells::Sensor m_testInput;
    cells::Sensor m_testSolution;
};

} // namespace synth