#include "UnitTester.h"

#include "app/Colors.h"
#include "app/Solver.h"
#include "cells/Cells.h"
#include "util/ArcTask.h"

#include <array>
#include <set>


namespace synth {

UnitTester::UnitTester(Logger& logger) :
    logger(logger)
{
    start();
}

struct TestCase;
static cells::Sensor convertTestCaseToSensor(const TestCase& testBoard);

struct TestCase
{
    std::string name;
    int width;
    int height;
    std::vector<int> pixels;
    std::function<void(const TestCase& testCase)> test;
};

static const TestCase testCase1("StretchTest", 3, 3, { 0, 7, 7, 7, 7, 7, 0, 7, 7 },
                                 [](const TestCase& testCase) {
                                    cells::Sensor sensor = convertTestCaseToSensor(testCase);
                                     PatchBoard patchBoard(sensor);
                                     patchBoard.process();
                                     std::shared_ptr<Patch> patch;
                                     for (std::shared_ptr<Patch> i : patchBoard.patches()) {
                                         if (i->size() == 7)
                                             patch = i;
                                     }
                                     loggerPtr->log(DEBUG) << "Patch:";
                                     loggerPtr->logBoard(DEBUG) << patch->toString() << "\n";
                                     DrawingBoard drawingBoard(25, 25);
                                     VectorShape vectorShape = patch->vectorize();
                                     vectorShape.firstPixel({ 3, 2 });

                                     drawingBoard.clear();
                                     drawingBoard.renderVectorShape(2, 1, vectorShape.stretch(1, 1));
                                     drawingBoard.renderVectorShape(2, 10, vectorShape.stretch(1, 2));
                                     drawingBoard.renderVectorShape(10, 3, vectorShape.stretch(2, 1));
                                     drawingBoard.renderVectorShape(10, 9, vectorShape.stretch(2, 2));
                                     drawingBoard.renderVectorShape(10, 18, vectorShape.stretch(3, 3));
                                     loggerPtr->log(DEBUG) << "DrawingBoard:";
                                     loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";

                                     loggerPtr->log(DEBUG) << "Number of patches found: " << patchBoard.patches().size();
                                 }
);

std::vector<TestCase> testCases = {
    testCase1
};

static cells::Sensor convertTestCaseToSensor(const TestCase& testBoard)
{
    cells::Sensor sensor;
    const int height = testBoard.height;
    const int width  = testBoard.width;
    sensor.width(width).height(height).init();

    assert(testBoard.pixels.size() == width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells::Pixel& pixel = sensor.getPixel(x, y);
            pixel.color         = color((arc::Colors)(testBoard.pixels[y * width + x]));
        }
    }

    return sensor;
}


void UnitTester::start()
{
    loggerPtr = &logger;
    logger.clearLogFile();
    logger.log(INFO) << "There are " << testCases.size() << " test cases";
    for (const TestCase& testCase : testCases) {
        testCase.test(testCase);
    }
}

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
} // namespace synth