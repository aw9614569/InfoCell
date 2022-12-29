#include "NewCells.h"

using namespace synth::newcell;

int main(int argc, char* argv[])
{
    StaticInitializations();

    CellValuePrinter valuePrinter;
    CellStructPrinter structPrinter;

    ClassCell colorClass("Color");
    MemberCell& redColorMember   = colorClass.createMember("red", NumberCell::classCell());
    MemberCell& greenColorMember = colorClass.createMember("green", NumberCell::classCell());
    MemberCell& blueColorMember  = colorClass.createMember("blue", NumberCell::classCell());
    NumberCell number_0(0);

    DataCell redColor(colorClass);
    redColor.connect(redColorMember, number_0);
    redColor.connect(greenColorMember, number_0);
    redColor.connect(blueColorMember, number_0);

    std::cout << valuePrinter.print(colorClass) << std::endl;
    std::cout << valuePrinter.print(redColorMember) << std::endl;
    std::cout << valuePrinter.print(redColor) << std::endl;
    std::cout << valuePrinter.print(number_0) << std::endl;

    std::cout << structPrinter.print(redColor) << std::endl;
    std::cout << structPrinter.print(number_0) << std::endl;
}