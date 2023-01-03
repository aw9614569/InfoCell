#include "Screen.h"

#include <nlohmann/json.hpp>

namespace synth {
namespace input {

const std::array<Pixel, 10> coloredPixels = {
    Pixel(0x00, 0x00, 0x00), /* black */
    Pixel(0x00, 0x74, 0xD9), /* blue */
    Pixel(0xFF, 0x41, 0x36), /* red */
    Pixel(0x2E, 0xCC, 0x40), /* green */
    Pixel(0xFF, 0xDC, 0x00), /* yellow */
    Pixel(0xAA, 0xAA, 0xAA), /* grey */
    Pixel(0xF0, 0x12, 0xBE), /* fuschia */
    Pixel(0xFF, 0x85, 0x1B), /* orange */
    Pixel(0x7F, 0xDB, 0xFF), /* teal */
    Pixel(0x87, 0x0C, 0x25)  /* brown */
};

Screen::Screen(const std::string& name) :
    m_name(name)
{
}

void Screen::loadFromJsonArray(const std::string& jsonStr)
{
    nlohmann::json arcMatrix = nlohmann::json::parse(jsonStr);
    size_t matrixHeight = arcMatrix.size();
    size_t matrixWidth  = arcMatrix[0].size();

    m_height = matrixHeight;
    m_width  = matrixWidth;

    int y = 0;
    for (auto inputRowIt = arcMatrix.begin(); inputRowIt != arcMatrix.end(); ++inputRowIt) {
        for (const int val : *inputRowIt) {
            m_pixels.push_back(coloredPixels[val]);
        }
    }
}

void Screen::loadFromArcJsonFile(const std::filesystem::path& path)
{
}

const std::string& Screen::name() const
{
    return m_name;
}

int Screen::width() const
{
    return m_width;
}

int Screen::height() const
{
    return m_height;
}

const std::vector<Pixel>& Screen::pixels() const
{
    return m_pixels;
}

} // namespace input
} // namespace synth
