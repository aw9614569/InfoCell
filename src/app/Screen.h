#include <filesystem>
#include <vector>

namespace synth {
namespace input {

// ============================================================================
class Pixel
{
public:
    Pixel(int red, int green, int blue) :
        m_red(red), m_green(green), m_blue(blue) { }

    int m_red;
    int m_green;
    int m_blue;
};

class Screen
{
public:
    Screen(const std::string& name);

    void loadFromJsonArray(const std::string& jsonStr);
    void loadFromArcJsonFile(const std::filesystem::path& path);
    const std::string& name() const;
    int width() const;
    int height() const;
    const std::vector<Pixel>& pixels() const;

private:
    std::string m_name;
    int m_width;
    int m_height;
    std::vector<Pixel> m_pixels;
};

} // namespace input
} // namespace synth