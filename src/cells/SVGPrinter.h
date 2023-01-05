#include "Cells.h"

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
    Printer();

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

protected:
    std::string printImpl(CellI& cell);
    std::string svgDrawBox(Point start, Point end);
    std::string svgDrawText(const std::string& str, Point point);

    int fontSize;
    std::string fontName;
    std::string fontPath;
};

} // namespace svg
} // namespace cells
} // namespace synth