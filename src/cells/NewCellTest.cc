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
    NumberCell number_255(255);

    DataCell redColor("redColor",  colorClass);
    redColor.connect(redColorMember, number_255);
    redColor.connect(greenColorMember, number_0);
    redColor.connect(blueColorMember, number_0);

    std::cout << colorClass.printAs(valuePrinter) << std::endl;
    std::cout << redColorMember.printAs(valuePrinter) << std::endl;
    std::cout << redColor.printAs(valuePrinter) << std::endl;
    std::cout << number_0.printAs(valuePrinter) << std::endl;
    std::cout << number_0.member(NumberCell::memberValue()).member(ListCell::memberFirst()).member(ListItemCell::memberValue()).printAs(valuePrinter) << std::endl;

    std::cout << redColor.printAs(structPrinter) << std::endl;
    std::cout << redColorMember.printAs(structPrinter) << std::endl;
    std::cout << colorClass.printAs(structPrinter) << std::endl;
    std::cout << number_255.printAs(structPrinter) << std::endl;
    std::cout << number_0.member(NumberCell::memberSign()).printAs(structPrinter) << std::endl;
    std::cout << number_0.member(NumberCell::memberValue()).printAs(structPrinter) << std::endl;
    std::cout << number_0.member(NumberCell::memberValue()).member(ListCell::memberFirst()).printAs(structPrinter) << std::endl;
    std::cout << number_0.member(NumberCell::memberValue()).member(ListCell::memberFirst()).member(ListItemCell::memberValue()).printAs(structPrinter) << std::endl;
    std::cout << number_0.member(NumberCell::memberValue()).member(ListCell::memberLast()).printAs(structPrinter) << std::endl;
    std::cout << number_255.member(NumberCell::memberValue()).member(ListCell::memberSize()).printAs(structPrinter) << std::endl;
}