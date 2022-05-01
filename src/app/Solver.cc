#include "Solver.h"
#include <array>

namespace synth {

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

Solver::Solver(Logger& logger, const ArcTask& arcTask) :
    logger(logger), m_arcTask(arcTask)
{
    solve();
}

void Solver::solve()
{
    logger.log(INFO) << "There are " << m_arcTask.m_demonstrations.size() << " demo tasks";
    unsigned int i = 1;
    for (const auto& arcDemo : m_arcTask.m_demonstrations) {
        const cells::Sensor& m_input = arcDemo.m_input;
        const cells::Sensor& m_output = arcDemo.m_output;

        logger.log(INFO) << " (" << i++ << ") mapping[" << m_input.m_width << ", " << m_input.m_height << "] to[" << m_output.m_width << ", " << m_output.m_height << "] ";
    }
    const cells::Sensor& m_input = m_arcTask.m_testInput;
    logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to ... ?";
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