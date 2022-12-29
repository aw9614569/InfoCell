#include "NewCells.h"

namespace synth {
namespace newcell {

std::unique_ptr<ClassCell> MemberCell::s_classCell;

// ============================================================================
MemberCell::MemberCell(const std::string& name, ClassCell& classCell) :
    m_name(name), m_classCell(classCell)
{
}

bool MemberCell::hasMember(CellI& member)
{
    return false; // TODO
}

CellI& MemberCell::member(CellI& member)
{
    return *this; // TODO
}

ClassCell& MemberCell::reflect()
{
    return *s_classCell;
}

std::string MemberCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

void MemberCell::staticInit()
{
    s_classCell.reset(new ClassCell("Class"));
}

const std::string& MemberCell::name() const
{
    return m_name;
}

ClassCell& MemberCell::classCell()
{
    return m_classCell;
}

// ============================================================================
std::unique_ptr<ClassCell> ClassCell::s_classCell;
std::unique_ptr<MemberCell> ClassCell::s_classMember;

ClassCell::ClassCell(const std::string& name) :
    m_name(name)
{
    referenceMember("class", *s_classMember);
}

bool ClassCell::hasMember(CellI& member)
{
    return false; // TODO
}

CellI& ClassCell::member(CellI& member)
{
    return *this; // TODO
}

ClassCell& ClassCell::reflect()
{
    return *s_classCell;
}

std::string ClassCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

void ClassCell::staticInit()
{
    s_classCell.reset(new ClassCell("Class"));
    s_classMember.reset(new MemberCell("class", *s_classCell));
}

const std::string& ClassCell::name() const
{
    return m_name;
}

MemberCell& ClassCell::createMember(const std::string& name, ClassCell& classCell)
{
    auto memberIt = m_memberRefs.find(name);
    if (memberIt != m_memberRefs.end()) {
        if (&memberIt->second->classCell() != &classCell) {
            throw "Member name already registered with an other class";
        }
        return *memberIt->second;
    } else {
        auto it = m_memberCells.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(name),
                                        std::forward_as_tuple(name, classCell));
        m_memberRefs[name] = &it.first->second;

        return it.first->second;
    }
}

void ClassCell::referenceMember(const std::string& name, MemberCell& memberCell)
{
    m_memberRefs[name] = &memberCell;
}

bool ClassCell::hasMember(const std::string& name) const
{
    return m_memberRefs.find(name) != m_memberRefs.end();
}

MemberCell& ClassCell::getMember(const std::string& name)
{
    auto findIt = m_memberRefs.find(name);
    if (findIt != m_memberRefs.end())
        throw "emptyMember";

    return *findIt->second;
}

std::map<std::string, MemberCell*>& ClassCell::members()
{
    return m_memberRefs;
}

ClassCell& ClassCell::classCell()
{
    return *s_classCell;
}

MemberCell& ClassCell::classMember()
{
    return *s_classMember;
}

// ============================================================================
ClassCell emptyClass("Empty");
DataCell DataCell::emptyDataCell(emptyClass);

DataCell::DataCell(ClassCell& classCell) : DataCell("", classCell)
{
}

DataCell::DataCell(const std::string& name, ClassCell& classCell) :
    m_name(name), m_classCell(classCell)
{
    m_members[&ClassCell::classMember()] = &classCell;
}

bool DataCell::hasMember(CellI& member)
{
    MemberCell* memberCell = static_cast<MemberCell*>(&member);
    if (memberCell == &ClassCell::classMember())
        return true;
    if (!memberCell)
        return false;

    return m_members.find(memberCell) != m_members.end();
}

CellI& DataCell::member(CellI& member)
{
    MemberCell* memberCell = static_cast<MemberCell*>(&member);
    if (!memberCell)
        return emptyDataCell;
    auto findIt = m_members.find(memberCell);
    if (findIt == m_members.end())
        return emptyDataCell;

    return *findIt->second;
}

ClassCell& DataCell::reflect()
{
    return m_classCell;
}

std::string DataCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

const std::string& DataCell::name() const
{
    return m_name;
}

std::map<MemberCell*, CellI*>& DataCell::members()
{
    return m_members;
}

void DataCell::connect(MemberCell& memberCell, CellI& value)
{
    m_members[&memberCell] = &value;
}

// ============================================================================
void DigitCells::staticInit()
{
}

ClassCell DigitCells::s_digitClassCell("Digit");

std::array<DataCell, 10> DigitCells::s_digits = {
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell,
    s_digitClassCell
};

// ============================================================================
ClassCell ListCell::s_classCell("List");
ClassCell ListCell::s_listItemClassCell("ListItem");

MemberCell* ListCell::s_listItemPrevMember  = nullptr;
MemberCell* ListCell::s_listItemNextMember  = nullptr;
MemberCell* ListCell::s_listItemValueMember = nullptr;

MemberCell* ListCell::s_firstMember = nullptr;
MemberCell* ListCell::s_lastMember  = nullptr;
MemberCell* ListCell::s_sizeMember  = nullptr;

template <typename T>
ListCell::ListCell(std::span<T> values)
{
    DataCell* prevListItemCell = nullptr;
    for (CellI* value : values) {
        auto& listItemCell = m_values.emplace_back(s_listItemClassCell);
        if (prevListItemCell == nullptr) {
            listItemCell.connect(*s_listItemPrevMember, DataCell::emptyDataCell);
        } else {
            listItemCell.connect(*s_listItemPrevMember, *prevListItemCell);
            prevListItemCell->connect(*s_listItemNextMember, listItemCell);
        }
        listItemCell.connect(*s_listItemNextMember, DataCell::emptyDataCell);
        listItemCell.connect(*s_listItemValueMember, *value);
        prevListItemCell = &listItemCell;
    }
}

bool ListCell::hasMember(CellI& member)
{
    if (&member == s_firstMember || &member == s_lastMember || &member == s_sizeMember) {
        return true;
    }
    return false;
}

CellI& ListCell::member(CellI& member)
{
    if (&member == s_firstMember) {
        if (m_values.empty()) {
            return DataCell::emptyDataCell;
        }
        return m_values.front();
    } else if (&member == s_lastMember) {
        if (m_values.empty()) {
            return DataCell::emptyDataCell;
        }
        return m_values.back();
    } else if (&member == s_sizeMember) {
        // TODO
        return DataCell::emptyDataCell;
    }

    return DataCell::emptyDataCell;
}

ClassCell& ListCell::reflect()
{
    return s_classCell;
}

std::string ListCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::vector<DataCell>& ListCell::values()
{
    return m_values;
}

void ListCell::staticInit()
{
    s_listItemPrevMember = &s_listItemClassCell.createMember("prev", s_listItemClassCell);
    s_listItemNextMember = &s_listItemClassCell.createMember("next", s_listItemClassCell);
    s_listItemValueMember = &s_listItemClassCell.createMember("value", s_listItemClassCell); // TODO we need the T from the List<T> here somehow

    s_firstMember = &s_classCell.createMember("first", s_listItemClassCell);
    s_lastMember  = &s_classCell.createMember("last", s_listItemClassCell);
    s_sizeMember  = &s_classCell.createMember("size", NumberCell::classCell());
}

ClassCell& ListCell::classCell()
{
    return s_classCell;
}

// ============================================================================
ClassCell NumberCell::s_classCell("Number");
MemberCell* NumberCell::s_valueMember = nullptr;
MemberCell* NumberCell::s_signMember = nullptr;

NumberCell::NumberCell(int value) :
    m_value(value)
{
}

bool NumberCell::hasMember(CellI& member)
{
    if (&member == s_valueMember || &member == s_signMember) {
        return true;
    }
    return false;
}

CellI& NumberCell::member(CellI& member)
{
    if (&member == s_valueMember) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsListCell.reset(new ListCell(std::span<DataCell*>(m_digits.begin(), m_digits.end())));
        }

        return *m_digitsListCell;
    } else {
        return DataCell::emptyDataCell;
    }
}

ClassCell& NumberCell::reflect()
{
    return s_classCell;
}

std::string NumberCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

int NumberCell::value() const
{
    return m_value;
}

void NumberCell::staticInit()
{
    s_valueMember = &s_classCell.createMember("value", ListCell::classCell());
    s_signMember = &s_classCell.createMember("sign", NumberCell::classCell()); // TODO
}

ClassCell& NumberCell::classCell()
{
    return s_classCell;
}

void NumberCell::calculateDigits()
{
    if (m_value == 0) {
        m_digits.push_back(&DigitCells::s_digits[0]);
        return;
    }
    int value = m_value;
    while (value) {
        m_digits.push_back(&DigitCells::s_digits[value % 10]);
        value /= 10;
    }
}

void StaticInitializations()
{
    MemberCell::staticInit();
    ClassCell::staticInit();
    DigitCells::staticInit();
    ListCell::staticInit();
    NumberCell::staticInit();
}

// ============================================================================
std::string CellValuePrinter::print(MemberCell& memberCell)
{
    std::stringstream ss;
    ss << memberCell.name() << ": " << memberCell.classCell().name();

    return ss.str();
}

std::string CellValuePrinter::print(ClassCell& classCell)
{
    std::stringstream ss;
    ss << "class " << classCell.name() << " { ";
    bool isFirst = true;
    for (auto& memberI : classCell.members()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << memberI.first << ": " << memberI.second->classCell().name();
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(DataCell& dataCell)
{
    std::stringstream ss;
    ss << dataCell.reflect().name() << " { ";
    bool isFirst = true;
    for (auto& memberI : dataCell.members()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << "." << memberI.first->name() << ": " << memberI.second->printAs(*this);
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(ListCell& listCell)
{
    std::stringstream ss;
    ss << "{";
    bool isFirst = true;
    for (auto& value : listCell.values()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << value.reflect().name();
    }
    ss << "}";

    return ss.str();
}

std::string CellValuePrinter::print(NumberCell& cell)
{
    std::stringstream ss;
    ss << "(Number) " << cell.value();

    return ss.str();
}
// ============================================================================
std::string CellStructPrinter::print(MemberCell& memberCell)
{
    return printImpl(memberCell);
}

std::string CellStructPrinter::print(ClassCell& classCell)
{
    return printImpl(classCell);
}

std::string CellStructPrinter::print(DataCell& dataCell)
{
    return printImpl(dataCell);
}

std::string CellStructPrinter::print(ListCell& listCell)
{
    return printImpl(listCell);
}

std::string CellStructPrinter::print(NumberCell& numberCell)
{
    return printImpl(numberCell);
}

std::string CellStructPrinter::printImpl(CellI& cell)
{
    std::stringstream ss;
    ClassCell& classCell = cell.reflect();
    ss << "(" << classCell.name() << ") ID" << (void*)this << std::endl;
    for (auto& memberI : classCell.members()) {
        if (!cell.hasMember(*memberI.second)) {
            continue;
        }
        ss << "    +--(" << memberI.first << ")--> ID" << &cell.member(*memberI.second) << std::endl;
    }
    return ss.str();
}

} // namespace newcell
} // namespace synth