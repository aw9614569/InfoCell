#include "NewCells.h"


namespace synth {
namespace newcell {
namespace svg {

class Point
{
public:
    int x;
    int y;
};

class Printer : public newcell::Printer
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
    std::string print(PixelRef& cell) override;
    std::string print(Sensor& cell) override;

protected:
    std::string printImpl(CellI& cell);
    std::string svgDrawBox(Point start, Point end);
    std::string svgDrawText(const std::string& str, Point point);

    int fontSize;
    std::string fontName;
    std::string fontPath;
};

} // namespace svg
} // namespace newcell
} // namespace synth