#include "NewCells.h"

using namespace synth::newcell;

int main(int argc, char* argv[])
{
    StaticInitializations();

    CellValuePrinter valuePrinter;
    CellStructPrinter structPrinter;

    Type colorClass("Color");
    Slot& redColorMember   = colorClass.createSlot("red", Number::type());
    Slot& greenColorMember = colorClass.createSlot("green", Number::type());
    Slot& blueColorMember  = colorClass.createSlot("blue", Number::type());
    Number number_0(0);
    Number number_255(255);

    Object redColor("redColor",  colorClass);
    redColor.connect(redColorMember, number_255);
    redColor.connect(greenColorMember, number_0);
    redColor.connect(blueColorMember, number_0);

    std::cout << colorClass.printAs(valuePrinter) << std::endl;
    std::cout << redColorMember.printAs(valuePrinter) << std::endl;
    std::cout << redColor.printAs(valuePrinter) << std::endl;
    std::cout << number_255.printAs(valuePrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).slot(List::slotFirst()).slot(ListItem::slotValue()).printAs(valuePrinter) << std::endl;

    std::cout << redColor.printAs(structPrinter) << std::endl;
    std::cout << redColorMember.printAs(structPrinter) << std::endl;
    std::cout << colorClass.printAs(structPrinter) << std::endl;
    std::cout << number_255.printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotSign()).printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).slot(List::slotFirst()).printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).slot(List::slotFirst()).slot(ListItem::slotValue()).printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).slot(List::slotLast()).printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).slot(List::slotSize()).printAs(structPrinter) << std::endl;
}