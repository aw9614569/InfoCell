#pragma once

#include <string>
#include <vector>

namespace synth {
namespace input {

// ============================================================================
class Color
{
public:
    Color(int red, int green, int blue) :
        m_red(red), m_green(green), m_blue(blue) { }

    bool operator==(const Color& rhs) const
    {
        return rhs.m_red == m_red && rhs.m_green == m_green && rhs.m_blue == m_blue;
    }

    bool operator<(const Color& rhs) const
    {
        return std::tie(m_red, m_green, m_blue) < std::tie(rhs.m_red, rhs.m_green, rhs.m_blue);
    }

    int m_red = 0;
    int m_green = 0;
    int m_blue = 0;
};

class Screen
{
public:
    Screen(const std::string& name);
    Screen(const std::string& name, const std::string& jsonStr);

    Screen& loadFromJsonArray(const std::string& jsonStr);
    Screen& loadFromVector(int width, int height, const std::vector<int>& pixels);
    const std::string& name() const;
    int width() const;
    int height() const;
    std::vector<Color>& pixels();
    const std::vector<Color>& pixels() const;

private:
    std::string m_name;
    int m_width = 0;
    int m_height = 0;
    std::vector<Color> m_pixels;
};

} // namespace input
} // namespace synth