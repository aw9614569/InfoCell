#include "ArcTask.h"

namespace synth {

ArcDemonstration::ArcDemonstration(const std::string& input, const std::string& output) :
    m_inputScreen("input"),
    m_outputScreen("output"),
    m_input(m_inputScreen.loadFromJsonArray(input)),
    m_output(m_outputScreen.loadFromJsonArray(output))
{
}

ArcTask::ArcTask(const std::string& input, const std::string& output) :
    m_inputScreen("input"),
    m_outputScreen("output"),
    m_testInput(m_inputScreen.loadFromJsonArray(input)),
    m_testSolution(m_outputScreen.loadFromJsonArray(output))
{
}

void ArcTask::addDemonstration(const std::string& input, const std::string& output)
{
    m_demonstrations.emplace_back(input, output);
}

} // namespace synth