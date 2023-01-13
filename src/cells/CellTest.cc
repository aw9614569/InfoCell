#include "Cells.h"
#include "SVGPrinter.h"
#include <fstream>

using namespace synth;
using namespace synth::cells;
using namespace synth::cells::data;


int main(int argc, char* argv[])
{
    svg::Printer svgPrinter(800, 600);
    CellValuePrinter valuePrinter;
    CellStructPrinter structPrinter;
    //    svgPrinter.test();

    StaticInitializations();

    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Sensor sensor(inputPicture);

//    svgFile << sensor.printAs(svgPrinter) << "\n";
    sensor[data::listOfPixels].printAs(svgPrinter);
    svgPrinter.writeFile("F:\\Devel\\ARC\\synth\\1.svg");


    Type Variable("Color",
                  { { "name", String::t(), data::coding::name },
                    { "value", Number::t(), data::coding::value } });
    Object var1("var1", Variable);

    control::pipeline::Node pipeNode1;
    control::Same sameOp(sensor, sensor, pipeNode1, data::coding::value);
    pipeNode1();
    std::cout << "SameOp: " << var1[data::coding::value].printAs(valuePrinter) << std::endl;
    std::cout << sensor[data::listOfPixels].printAs(valuePrinter) << "\n";


    Object colorRed(Type::anyType());
    Object colorGreen(Type::anyType());
    Object colorBlue(Type::anyType());

    Type colorClass("Color",
                    { { "red", Number::t(), colorRed },
                      { "green", Number::t(), colorGreen },
                      { "blue", Number::t(), colorBlue } });

    Object redColor("redColor", colorClass);
    redColor.set(colorRed, Numbers::get(255));
    redColor.set(colorGreen, Numbers::get(0));
    redColor.set(colorBlue, Numbers::get(0));

    Number& number_0   = Numbers::get(0);
    Number& number_255 = Numbers::get(255);

    std::cout << colorClass.printAs(valuePrinter) << std::endl;
    std::cout << colorClass.getSlot("red").printAs(valuePrinter) << std::endl;
    std::cout << redColor.printAs(valuePrinter) << std::endl;
    std::cout << number_255.printAs(valuePrinter) << std::endl;
    std::cout << number_255[value][first][value].printAs(valuePrinter) << std::endl;

    std::cout << redColor.printAs(structPrinter) << std::endl;
    std::cout << colorClass.getSlot("red").printAs(structPrinter) << std::endl;
    std::cout << colorClass.printAs(structPrinter) << std::endl;
    std::cout << number_255.printAs(structPrinter) << std::endl;
    std::cout << number_255[sign].printAs(structPrinter) << std::endl;
    std::cout << number_255[value].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][first].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][first][value].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][last].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][size].printAs(structPrinter) << std::endl;
}