#include "ArcTask.h"

#include "app/Picture.h"
#include <array>
#include <format>

using namespace nlohmann;
using namespace synth::input;

namespace synth {

static const std::array<Color, 10> cellColors = {
    Color(0x00, 0x00, 0x00), /* black */
    Color(0x00, 0x74, 0xD9), /* blue */
    Color(0xFF, 0x41, 0x36), /* red */
    Color(0x2E, 0xCC, 0x40), /* green */
    Color(0xFF, 0xDC, 0x00), /* yellow */
    Color(0xAA, 0xAA, 0xAA), /* grey */
    Color(0xF0, 0x12, 0xBE), /* fuschia */
    Color(0xFF, 0x85, 0x1B), /* orange */
    Color(0x7F, 0xDB, 0xFF), /* teal */
    Color(0x87, 0x0C, 0x25)  /* brown */
};

ArcDemonstration::ArcDemonstration(int number, const std::string& input, const std::string& output) :
    m_number(number),
    m_inputPicture(std::format("Train input {}", number)),
    m_outputPicture(std::format("Train output {}", number)),
    m_input(m_inputPicture.loadFromJsonArray(input)),
    m_output(m_outputPicture.loadFromJsonArray(output))
{
}

ArcTask::ArcTask(const nlohmann::json& jsonArcFile) :
    m_inputPicture("Test input"),
    m_outputPicture("Test solution"),
    m_testInput(m_inputPicture.loadFromJsonArray(to_string(jsonArcFile["/test/0/input"_json_pointer]))),
    m_testSolution(m_outputPicture.loadFromJsonArray(to_string(jsonArcFile["/test/0/output"_json_pointer])))
{
    const nlohmann::json& jsonTrainSet = jsonArcFile.at("train");
    m_demonstrations.reserve(jsonTrainSet.size());
    int number = 1;
    for (const auto& train : jsonTrainSet) {
        m_demonstrations.emplace_back(number++, to_string(train.at("input")), to_string(train.at("output")));
    }
}

} // namespace synth