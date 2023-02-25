#pragma once
#include <sstream>

#include "Cells.h"

namespace synth {
namespace cells {

class CellValuePrinter : public Visitor
{
public:
    void visit(Slot& cell) override;
    void visit(Type& cell) override;
    void visit(Group::MemberList::Item& cell) override;
    void visit(Group::MemberList& cell) override;
    void visit(Group::MemberIndex::Type::Slots::SlotList::Item& cell) override;
    void visit(Group::MemberIndex::Type::Slots::SlotList& cell) override;
    void visit(Group::MemberIndex::Type::Slots::SlotIndex& cell) override;
    void visit(Group::MemberIndex::Type::Slots& cell) override;
    void visit(Group::MemberIndex::Type::Slot& cell) override;
    void visit(Group::MemberIndex::Type& cell) override;
    void visit(Group::MemberIndex& cell) override;
    void visit(Group& cell) override;
    void visit(Object& cell) override;
    void visit(List::Item& cell) override;
    void visit(List& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Picture& cell) override;

    std::string print() const;

protected:
    void printImpl(CellI& list);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace synth