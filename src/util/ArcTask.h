#pragma once

#include <nlohmann/json.hpp>
#include "app/Picture.h"
#include "cells/Brain.h"

namespace synth {
namespace cells {
namespace brain {
class Brain;
} // namespace brain
} // namespace cells

class ArcDemonstration
{
public:
    ArcDemonstration(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output);

    const int m_number;
    input::Grid m_inputPicture;
    input::Grid m_outputPicture;
    cells::hybrid::Picture m_input;
    cells::hybrid::Picture m_output;
};

class ArcTask
{
public:
    ArcTask(cells::brain::Brain& kb, const nlohmann::json& jsonArcFile);

    std::vector<ArcDemonstration> m_demonstrations;
    std::vector<cells::Object> m_exampleObjects;
    input::Grid m_inputPicture;
    input::Grid m_outputPicture;
    cells::hybrid::Picture m_challenge;
    cells::hybrid::Picture m_solution;
    cells::CellI& m_taskStruct;
    cells::CellI& m_demonstrationStruct;
    cells::Object m_task;
    cells::List m_examples;
};

class ArcTaskExample
{
public:
    ArcTaskExample(cells::brain::Brain& kb, int number, const std::string& input);
    ArcTaskExample(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output);

    const int m_number;
    input::Grid m_inputGrid;
    std::unique_ptr<input::Grid> m_outputGrid;
    cells::hybrid::arc::Grid m_input;
    std::unique_ptr<cells::hybrid::arc::Grid> m_output;
};

class ArcPrizeTask
{
public:
    ArcPrizeTask(cells::brain::Brain& kb, const std::string& id, const nlohmann::json& jsonTask);

    std::string m_id;
    std::vector<ArcTaskExample> m_examples;
    std::vector<ArcTaskExample> m_tests;

    cells::CellI& m_cellTaskStruct;
    cells::CellI& m_cellExampleStruct;
    cells::Object m_cellTask;

    std::vector<cells::Object> m_cellExamples;
    std::vector<cells::Object> m_cellTests;

    cells::List m_cellExamplesList;
    cells::List m_cellTestsList;
};

class TaskSet
{
public:
    TaskSet(cells::brain::Brain& kb, const std::string& filePath);

    std::map<std::string, ArcPrizeTask> m_tasks;
};

} // namespace synth