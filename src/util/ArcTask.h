#pragma once

#include <nlohmann/json.hpp>
#include "app/Picture.h"
#include "cells/Cells.h"

namespace synth {

class ArcDemonstration
{
public:
    ArcDemonstration(int number, const std::string& input, const std::string& output);

    const int m_number;
    input::Picture m_inputPicture;
    input::Picture m_outputPicture;
    cells::hybrid::Sensor m_input;
    cells::hybrid::Sensor m_output;
};

class ArcTask
{
public:
    ArcTask(const nlohmann::json& jsonArcFile);

    std::vector<ArcDemonstration> m_demonstrations;
    input::Picture m_inputPicture;
    input::Picture m_outputPicture;
    cells::hybrid::Sensor m_testInput;
    cells::hybrid::Sensor m_testSolution;
};

} // namespace synth