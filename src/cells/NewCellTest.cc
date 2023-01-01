#include "NewCells.h"

using namespace synth::newcell;

int main(int argc, char* argv[])
{
    StaticInitializations();

    CellValuePrinter valuePrinter;
    CellStructPrinter structPrinter;

    Type colorClass("Color");
    Slot& redColorMember   = colorClass.createSlot("red", Number::t());
    Slot& greenColorMember = colorClass.createSlot("green", Number::t());
    Slot& blueColorMember  = colorClass.createSlot("blue", Number::t());

    Object redColor("redColor", colorClass);
    redColor.connect(redColorMember, Numbers::get(255));
    redColor.connect(greenColorMember, Numbers::get(0));
    redColor.connect(blueColorMember, Numbers::get(0));

    Number& number_0   = Numbers::get(0);
    Number& number_255 = Numbers::get(255);

    std::cout << colorClass.printAs(valuePrinter) << std::endl;
    std::cout << redColorMember.printAs(valuePrinter) << std::endl;
    std::cout << redColor.printAs(valuePrinter) << std::endl;
    std::cout << number_255.printAs(valuePrinter) << std::endl;
    List& number_255_value = static_cast<List&>(number_255.slot(Number::slotValue()));
    //std::cout << number_255.slot(Number::slotValue()).slot(List::slotFirst()).slot(ListItem::slotValue()).printAs(valuePrinter) << std::endl;

    std::cout << redColor.printAs(structPrinter) << std::endl;
    std::cout << redColorMember.printAs(structPrinter) << std::endl;
    std::cout << colorClass.printAs(structPrinter) << std::endl;
    std::cout << number_255.printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotSign()).printAs(structPrinter) << std::endl;
    std::cout << number_255.slot(Number::slotValue()).printAs(structPrinter) << std::endl;
//    std::cout << number_255.slot(Number::slotValue()).slot(List::slotFirst()).printAs(structPrinter) << std::endl;
//    std::cout << number_255.slot(Number::slotValue()).slot(List::slotFirst()).slot(ListItem::slotValue()).printAs(structPrinter) << std::endl;
//    std::cout << number_255.slot(Number::slotValue()).slot(List::slotLast()).printAs(structPrinter) << std::endl;
//    std::cout << number_255.slot(Number::slotValue()).slot(List::slotSize()).printAs(structPrinter) << std::endl;
}