#include "Cells.h"
#include "Brain.h"

namespace synth::cells::hybrid::arc {

class Shaper : public CellI
{
public:
    Shaper(brain::Brain& kb, cells::hybrid::arc::Grid& grid, CellI& ShapeStruct) :
        CellI(kb),
        m_width(grid.widthCell()),
        m_height(grid.heightCell()),
        m_grid(grid),
        m_shapePixels(kb, kb.std.Number, kb.std.Number),
        m_shapes(kb, kb.std.Number),
        m_shapeMap(kb, kb.std.Number, ShapeStruct),
        m_inputPixels(kb, kb.std.Pixel)
    {
        processInputPixels();
    }

    void processInputPixels()
    {
        std::vector<cells::hybrid::arc::Pixel>& pixels = m_grid.pixels();
        for (auto& pixel : pixels) {
            m_inputPixels.add(pixel);
        }
    }

    void process()
    {
        Number* shapeIdPtr = &static_cast<Number&>(kb._1_);
        while (!m_inputPixels.empty()) {
            CellI& firstPixel = m_inputPixels.first();
        }
    }

    Number& m_width;
    Number& m_height;
    cells::hybrid::arc::Grid& m_grid;
    Map m_shapePixels;
    List m_shapes;
    Map m_shapeMap;
    Set m_inputPixels;
};
} // namespace synth::cells::hybrid::arc