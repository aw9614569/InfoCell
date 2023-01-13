#include <filesystem>

#include "Cells.h"
#include "fsvgui/dom/elements.h"

namespace synth {
namespace cells {
namespace svg {

class Point
{
public:
    int x;
    int y;
};

class Printer : public cells::Printer
{
public:
    Printer(int width, int height);

    void test();
    std::string print(Slot& cell) override;
    std::string print(Type& cell) override;
    std::string print(Object& cell) override;
    std::string print(ListItem& cell) override;
    std::string print(List& cell) override;
    std::string print(Number& cell) override;
    std::string print(String& cell) override;
    std::string print(hybrid::Color& cell) override;
    std::string print(hybrid::Pixel& cell) override;
    std::string print(hybrid::Sensor& cell) override;

    void writeFile(const std::filesystem::path& path);

protected:
    std::string printImpl(CellI& cell);

    int m_fontSize;
    std::string m_fontName;
    std::string m_fontPath;
    fsvgui::Screen m_screen;
};

} // namespace svg
} // namespace cells
} // namespace synth