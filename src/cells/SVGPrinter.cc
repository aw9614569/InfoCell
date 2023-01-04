#include "SVGPrinter.h"

#include <format>
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H

class BoxSize
{
public:
    int m_width;
    int m_height;
};
static BoxSize stringBB(const std::string& str, int fontSize, const std::string& fontName, const std::string& fontPath)
{
    // Freetype default is 72 DPI.
    const int DISPLAY_DPI = 96;
    FT_Library library;
    FT_Face face;
    FT_Error error = FT_Init_FreeType(&library); /* initialize library */

    /* error handling omitted */
    error = FT_New_Face(library, fontPath.c_str(), 0, &face); /* create face object */

    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size(face, fontSize * 64, fontSize * 64, DISPLAY_DPI, DISPLAY_DPI); /* set character size */

    FT_Bool use_kerning    = FT_HAS_KERNING(face);
    FT_UInt previous_glyph = 0;

    int pen_x        = 0;
    int maxUpperSize = 0;
    int maxBelowSize = 0;

    for (auto ch : str) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, ch);

        if (use_kerning && previous_glyph && glyph_index) {
            FT_Vector delta;
            FT_Get_Kerning(face, previous_glyph, glyph_index, FT_KERNING_DEFAULT, &delta);
            pen_x += delta.x >> 6;
        }

        error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
        if (error)
            continue; /* ignore errors */

        // Dividing by 64 and increment pen position.
        pen_x += face->glyph->advance.x >> 6;

        int belowSize = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >> 6;
        if (belowSize > maxBelowSize)
            maxBelowSize = belowSize;

        int upperSize = face->glyph->metrics.horiBearingY >> 6;
        if (upperSize > maxUpperSize)
            maxUpperSize = upperSize;

        // Record current glyph index
        previous_glyph = glyph_index;
    }

    int startX        = 100;
    static int startY = 100;

    int maxY = maxUpperSize + maxBelowSize;

    std::cout << std::format("<polygon points=\"{},{} {},{} {},{} {},{}\" style=\"fill:lime;stroke:purple;stroke-width:1\"/>", startX, startY, startX + pen_x, startY, startX + pen_x, startY - maxY, startX, startY - maxY) << std::endl;
    std::cout << std::format("<text x=\"{}\" y=\"{}\" fill=\"black\" font-size=\"{}pt\" font-family=\"{}\">{}</text>", startX, startY - maxBelowSize, fontSize, fontName, str) << std::endl;
    startY += 10 + maxY;

    return { startX + pen_x, maxY };
}

namespace synth {
namespace cells {
namespace svg {

Printer::Printer()
{
    int fontSize         = 16;
    std::string fontName = "Times New Roman";
    std::string fontPath = "C:\\Windows\\Fonts\\times.ttf";
}

std::string Printer::svgDrawBox(Point start, Point end)
{
    int width  = end.x - start.x;
    int height = end.y - start.y;
    return std::format("<polygon points=\"{},{} {},{} {},{} {},{}\" style=\"fill:lime;stroke:purple;stroke-width:1\"/>", start.x, start.y, start.x + width, start.y, start.x + width, start.y + height, start.x, start.y + height);
}

std::string Printer::svgDrawText(const std::string& str, Point point)
{
    return std::format("<text x=\"{}\" y=\"{}\" fill=\"black\" font-size=\"{}pt\" font-family=\"{}\">{}</text>", point.x, point.y, fontSize, fontName, str);
}

std::string Printer::print(Slot& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(Type& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(Object& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(ListItem& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(List& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(Number& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(String& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(PixelRef& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(Sensor& cell)
{
    std::stringstream ss;
    return ss.str();
}

void Printer::test()
{
    //    stringBB("ab", 16, "Arial", "C:\\Windows\\Fonts\\arial.ttf");
    stringBB("Hg", 16, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("g", 16, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("Lorem ipsum dolor sit amet", 18, "Arial", "C:\\Windows\\Fonts\\arial.ttf");
    stringBB("World!", 20, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("fgsdfgsd", 20, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("lllllllll", 20, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("Hello Worsdfgsdgfsdfld!", 22, "Arial", "C:\\Windows\\Fonts\\arial.ttf");
    stringBB("Hellgsdfgdso World!", 24, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("QWERTYUIOPASDFGHJKLZXCVBNM", 26, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("qwertyuiopasdfghjklzxcvbnm", 28, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
}

} // namespace svg
} // namespace cells
} // namespace synth
