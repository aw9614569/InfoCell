#include "UnitTester.h"

#include "app/Colors.h"
#include "cells/Cells.h"
#include "util/ArcTask.h"

#include <array>
#include <set>


namespace synth {

static Logger* loggerPtr = nullptr;

UnitTester::UnitTester(Logger& logger) :
    logger(logger)
{
    start();
}

void UnitTester::start()
{
    loggerPtr = &logger;
    logger.clearLogFile();
#if 0
    logger.log(INFO) << "There are " << m_arcTask.m_demonstrations.size() << " demo tasks";
    unsigned int i = 1;
    for (const auto& arcDemo : m_arcTask.m_demonstrations) {
        const cells::Sensor& m_input  = arcDemo.m_input;
        const cells::Sensor& m_output = arcDemo.m_output;
        //        logger.log(INFO) << " (" << i << ") mapping[" << m_input.m_width << ", " << m_input.m_height << "] to[" << m_output.m_width << ", " << m_output.m_height << "] ";
        solveOne(m_input);
        break;
        solveOne(m_output);
        i++;
    }
    //    solveOne(m_arcTask.m_testInput);

    const cells::Sensor& m_input = m_arcTask.m_testInput;
    logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to ... ?";
#endif
}

#if 0
void Solver::solveOne(const cells::Sensor& sensor)
{
    PatchBoard patchBoard(sensor);
    patchBoard.process();
    std::shared_ptr<Patch> patch;
    for (std::shared_ptr<Patch> i : patchBoard.patches()) {
        if (i->size() == 7)
            patch = i;
    }
    logger.log(DEBUG) << "Patch:";
    loggerPtr->logBoard(DEBUG) << patch->toString() << "\n";
    DrawingBoard drawingBoard(25, 25);
    VectorShape vectorShape = patch->vectorize();
    vectorShape.firstPixel({ 3, 2 });

    for (int i = 0; i < 8; ++i) {
        drawingBoard.clear();
        drawingBoard.renderVectorShape(2, 1, vectorShape.stretch(1, 1));
        drawingBoard.renderVectorShape(2, 10, vectorShape.stretch(1, 2));
        drawingBoard.renderVectorShape(10, 3, vectorShape.stretch(2, 1));
        drawingBoard.renderVectorShape(10, 9, vectorShape.stretch(2, 2));
        drawingBoard.renderVectorShape(10, 18, vectorShape.stretch(3, 3));
#if 0
        VectorShape rotatedShape = vectorShape.rotate((RotationDir)i);
        drawingBoard.renderVectorShape(rotatedShape);
        drawingBoard.renderVectorShape(rotatedShape.mirror({ 3, 0 }, RotationDir::Degree_0));
        drawingBoard.renderVectorShape(rotatedShape.mirror({ 0, 3 }, RotationDir::Degree_90));
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_0);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_45);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_90);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_135);
#endif
        logger.log(DEBUG) << "DrawingBoard:";
        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
        break;
    }

    logger.log(DEBUG) << "Number of patches found: " << patchBoard.patches().size();
}
#endif

} // namespace synth