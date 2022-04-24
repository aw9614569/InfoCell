#include <array>
#include <filesystem>
namespace fs = std::filesystem;

#include "App.h"
#include "cells/Cells.h"

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Slider
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include <ftxui/dom/table.hpp>

using namespace ftxui;

namespace synth {

const std::array<Color, 10> App::arcColors = {
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

void App::init(int argc, char* argv[])
{
    if (argc == 1) {
        arcDbPath = "F:\\Devel\\ARC\\ARC\\data\\training\\";
//        arcDbPath = "F:\\Devel\\ARC\\ARC\\data\\evaluation\\";
    } else {
        arcDbPath = argv[1];
    }

    arcFileNames.clear();
    for (const auto& entry : fs::directory_iterator(arcDbPath)) {
        arcFileNames.push_back(entry.path().filename().string());
    }

    loadArcFileByFileIndex();
}

Element colorTile(Color p_color)
{
    return text("") | size(WIDTH, EQUAL, 2) | size(HEIGHT, EQUAL, 1) | bgcolor(p_color);
}

Element colorTile(ArcColors arcColor)
{
    return colorTile(App::arcColors[(int)arcColor]);
}

std::string App::getArcFilePathFromIndex(int index)
{
    return arcDbPath + arcFileNames[selectedArcFileIndex];
}

void App::loadArcFileByFileIndex()
{
    arcDb.load(getArcFilePathFromIndex(selectedArcFileIndex));
    renderArcTaskDemonstration();
    renderArcTestInputGrid();
}

void App::renderArcTaskDemonstration()
{
    auto& jTrain = arcDb.jf.at("train");
    int i        = 0;
    std::vector<Elements> arcTaskDemonstrationTableData;
    arcTaskDemonstrationTableData.push_back({
        text("Input") | center,
        text("Output") | center,
    });
    for (const auto& train : arcDb.jf.at("train")) {
        auto& inputRow  = train.at("input");
        auto& outputRow = train.at("output");
        Elements arcSet;

        Elements arcSetInputLines;
        for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
            Elements arcSetInputLine;
            for (const int val : *inputRowIt) {
                arcSetInputLine.push_back(colorTile((ArcColors)val));
            }
            arcSetInputLines.push_back(hbox(arcSetInputLine));
        }
        Element arcSetInputGrid = vbox(arcSetInputLines);

        Elements arcSetOutputLines;
        for (auto outputRowIt = outputRow.begin(); outputRowIt != outputRow.end(); ++outputRowIt) {
            Elements arcSetOutputLine;
            for (const int val : *outputRowIt) {
                arcSetOutputLine.push_back(colorTile((ArcColors)val));
            }
            arcSetOutputLines.push_back(hbox(arcSetOutputLine));
        }
        Element arcSetOutputGrid = vbox(arcSetOutputLines);

        arcTaskDemonstrationTableData.push_back({ arcSetInputGrid, arcSetOutputGrid });
    }

    Table arcTaskDemonstrationTable(arcTaskDemonstrationTableData);
    for (int rowIndex = 0; rowIndex + 1 < arcTaskDemonstrationTableData.size(); ++rowIndex) {
        arcTaskDemonstrationTable.SelectRow(rowIndex).BorderBottom(LIGHT);
    }
    arcTaskDemonstrationTable.SelectColumn(0).BorderRight(LIGHT);

    arcTaskDemonstration = arcTaskDemonstrationTable.Render();
}

void App::renderArcTestInputGrid()
{
    auto& inputRow = arcDb.jf["/test/0/input"_json_pointer];

    Elements arcSetInputLines;
    for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
        Elements arcSetInputLine;
        for (const int val : *inputRowIt) {
            arcSetInputLine.push_back(colorTile((ArcColors)val));
        }
        arcSetInputLines.push_back(hbox(arcSetInputLine));
    }
    arcTestInputGrid = vbox({ text("Test") | center, separator(), vbox(arcSetInputLines) });
}

void App::run()
{
    Components menuItems;
    for (const auto& fileName : arcFileNames) {
        menuItems.push_back(MenuEntry(fileName));
    }
    auto menu = Container::Vertical(menuItems, &selectedArcFileIndex);
    auto buttons = Container::Horizontal({
        Button("Solve", [&] { solve(); }),
        Button("Quit", [&] { exit(0); })
    });

    FlexboxConfig flexConfig;
    flexConfig.direction       = static_cast<FlexboxConfig::Direction>(0);
    flexConfig.wrap            = static_cast<FlexboxConfig::Wrap>(0);
    flexConfig.justify_content = static_cast<FlexboxConfig::JustifyContent>(0);
    flexConfig.align_items     = static_cast<FlexboxConfig::AlignItems>(0);
    flexConfig.align_content   = static_cast<FlexboxConfig::AlignContent>(0);

  auto container = Container::Vertical({
        buttons,
        menu
    });

    auto renderer = Renderer(container, [&] {
        if (previusSelectedArcFileIndex != selectedArcFileIndex) {
            loadArcFileByFileIndex();
            previusSelectedArcFileIndex = selectedArcFileIndex;
        }

        auto ret = flexbox({ vbox({ hbox(text("selected = "), text(arcFileNames[selectedArcFileIndex])),
                                    separator(),
                                    buttons->Render(),
                                    separator(),
                                    menu->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 25) })
                                 | border,
                             arcTaskDemonstration | border,
                             arcTestInputGrid | border }, flexConfig);
        return ret;
    });

    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(renderer);
}

const std::array<cells::Color, 10> cellColors = {
    cells::Color(0x00, 0x00, 0x00), /* black */
    cells::Color(0x00, 0x74, 0xD9), /* blue */
    cells::Color(0xFF, 0x41, 0x36), /* red */
    cells::Color(0x2E, 0xCC, 0x40), /* green */
    cells::Color(0xFF, 0xDC, 0x00), /* yellow */
    cells::Color(0xAA, 0xAA, 0xAA), /* grey */
    cells::Color(0xF0, 0x12, 0xBE), /* fuschia */
    cells::Color(0xFF, 0x85, 0x1B), /* orange */
    cells::Color(0x7F, 0xDB, 0xFF), /* teal */
    cells::Color(0x87, 0x0C, 0x25)  /* brown */
};

static std::shared_ptr<cells::Cell> convertJsonMatrixToCell(const nlohmann::json& jsonDb, const std::string& path)
{
    nlohmann::json::json_pointer jptr(path);

    auto& jMatrix = jsonDb[jptr];

    std::shared_ptr<cells::Cell> picture;
    cells::Sensor sensor;
    int matrixHeight = jMatrix.size();
    int matrixWidth  = jMatrix[0].size();

    sensor.height(matrixHeight).width(matrixWidth);
    sensor.init();

    int y = 0;
    for (auto inputRowIt = jMatrix.begin(); inputRowIt != jMatrix.end(); ++inputRowIt) {
        int x = 0;
        for (const int val : *inputRowIt) {
            cells::Pixel& pixel = sensor.getPixel(x, y);
            pixel.color         = cellColors[val];
        }
    }

    std::shared_ptr<cells::Cell> ret;
    return ret;
}

static void solvePair(const cells::Cell& input, const cells::Cell& output)
{
}

void App::solve()
{
    auto input = convertJsonMatrixToCell(arcDb.jf, "/train/0/input");
    auto output = convertJsonMatrixToCell(arcDb.jf, "/train/0/output");
    solvePair(*input, *output);
}

/*

A program célja, hogy két pixelmátrixból megtalálja a transzformációs algoritmust.
A feladatok során mindig található egy transzformációs algoritmus, ami egyik mátrixból előállítja a másikat.

Első lépés, hogy a pixel mátrixban azonosítsuk az azonos színű alakzatokat. Az alakzat akár 1 pixeles is lehet.
Az egyik kitüntetett alakzat a "háttér" alakzat, ez jellemzően fekete. Sok példában, de nem mindben az alakzatok egy háttér előtt vannak.

Egy 2D alakzatot lehet forgatni, kicsinyíteni, nagyítani, mozgatni, átszínezni valamilyen szabály szerint.

Pixels => Shapes

*/

} // namespace synth