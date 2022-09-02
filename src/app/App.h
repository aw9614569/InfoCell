#pragma once

#include "Colors.h"
#include "util/Logging.h"
#include "ftxui/component/component.hpp" // for Slider

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <sstream>

namespace synth {

class App
{
public:
    App() :
        solverLogger(m_solvingLogs) { }
    void init(int argc, char* argv[]);
    void run();
    static const std::array<ftxui::Color, 11> arcColors;

private:
    std::string getArcFilePathFromIndex(int index);
    void loadArcFile(const std::string& filename);
    void loadArcFileByFileIndex();
    void renderArcTaskDemonstration();
    void renderArcTestInputGrid();
    void solve();

    std::vector<std::string> m_arcFileNames;
    std::vector<LogMessage> m_solvingLogs;
    Logger solverLogger;

    nlohmann::json m_arcJsonTask;
    std::string m_arcFilePath;
    ftxui::Element m_arcTaskDemonstration;
    ftxui::Element m_arcTestInputGrid;
    int m_selectedArcFileIndex        = 0;
    int m_previusSelectedArcFileIndex = 0;
};

} // namespace synth