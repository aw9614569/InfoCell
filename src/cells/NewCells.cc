#include "NewCells.h"

#include <format>

#pragma warning(disable : 4996)
#include <utf8.h>

namespace synth {
namespace newcell {

// ============================================================================
std::unique_ptr<ClassCell> MemberCell::s_classCell;
MemberCell* MemberCell::s_memberConnectWith = nullptr;
MemberCell* MemberCell::s_memberName        = nullptr;
MemberCell* MemberCell::s_memberSemantic    = nullptr;

MemberCell::MemberCell(const std::string& name, ClassCell& classCell) :
    m_name(name), m_connectionClass(classCell)
{
}

bool MemberCell::hasMember(CellI& member)
{
    if (&member == &ClassCell::memberClass() || &member == s_memberConnectWith || &member == s_memberName) {
        return true;
    }
    return false;
}

CellI& MemberCell::member(CellI& member)
{
    if (&member == &ClassCell::memberClass()) {
        return *s_classCell;
    }
    if (&member == s_memberConnectWith) {
        return m_connectionClass;
    }
    if (&member == s_memberName) {
        if (!m_classNameListCell) {
            m_classNameListCell.reset(new StringCell(m_name));
        }
        return *m_classNameListCell;
    }
    if (&member == s_memberSemantic) {
        return *s_memberSemantic;
    }

    return DataCell::emptyDataCell();
}

ClassCell& MemberCell::reflect()
{
    return *s_classCell;
}

std::string MemberCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::string MemberCell::name() const
{
    return m_name;
}

void MemberCell::staticInit()
{
    s_classCell.reset(new ClassCell("Member"));
}

void MemberCell::staticInitMembers()
{
    s_memberConnectWith = &s_classCell->createMember("connectWith", ClassCell::classCell());
    s_memberName        = &s_classCell->createMember("name", StringCell::classCell());
    s_memberSemantic    = &s_classCell->createMember("semantic", ClassCell::anyClassCell());
}

MemberCell& MemberCell::memberName()
{
    return *s_memberName;
}

MemberCell& MemberCell::memberConnectWith()
{
    return *s_memberConnectWith;
}

MemberCell& MemberCell::memberSemantic()
{
    return *s_memberSemantic;
}

ClassCell& MemberCell::connectionClass()
{
    return m_connectionClass;
}

// ============================================================================
std::unique_ptr<ClassCell> ClassCell::s_classCell;
MemberCell* ClassCell::s_memberClass   = nullptr;
MemberCell* ClassCell::s_memberMembers = nullptr;
std::unique_ptr<ClassCell> ClassCell::s_anyClassCell;

ClassCell::ClassCell(const std::string& name) :
    m_name(name)
{
    if (!s_memberClass) {
        return;
    }
    referenceMember("class", *s_memberClass);
}

bool ClassCell::hasMember(CellI& member)
{
    if (&member == &memberClass() || &member == s_memberMembers) {
        return true;
    }

    return false;
}

CellI& ClassCell::member(CellI& member)
{
    if (&member == &memberClass()) {
        return *s_classCell;
    }
    if (&member == s_memberMembers) {
        if (!m_membersListCell)
            m_membersListCell.reset(new ListCell(m_memberRefs));

        return *m_membersListCell;
    }

    return DataCell::emptyDataCell();
}

ClassCell& ClassCell::reflect()
{
    return *s_classCell;
}

std::string ClassCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::string ClassCell::name() const
{
    return m_name;
}

void ClassCell::staticInit()
{
    s_classCell.reset(new ClassCell("Class"));
    s_memberClass = &s_classCell->createMember("class", *s_classCell);
    s_classCell->referenceMember("class", *s_memberClass);
    s_anyClassCell.reset(new ClassCell("AnyClass"));
}

void ClassCell::staticInitMembers()
{
    s_memberMembers = &s_classCell->createMember("members", ListCell::classCell());
}

MemberCell& ClassCell::createMember(const std::string& name, ClassCell& classCell)
{
    auto memberIt = m_memberRefs.find(name);
    if (memberIt != m_memberRefs.end()) {
        if (&memberIt->second->connectionClass() != &classCell) {
            throw "Member name already registered with an other class";
        }
        return *memberIt->second;
    } else {
        auto it            = m_memberCells.emplace(std::piecewise_construct,
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

MemberCell& ClassCell::memberClass()
{
    return *s_memberClass;
}

MemberCell& ClassCell::memberMembers()
{
    return *s_memberMembers;
}

ClassCell& ClassCell::anyClassCell()
{
    return *s_anyClassCell;
}

// ============================================================================
std::unique_ptr<DataCell> DataCell::s_emptyDataCell;

DataCell::DataCell(ClassCell& classCell) :
    DataCell("", classCell)
{
}

DataCell::DataCell(const std::string& name, ClassCell& classCell) :
    m_name(name), m_classCell(classCell)
{
    m_members[&ClassCell::memberClass()] = &classCell;
}

bool DataCell::hasMember(CellI& member)
{
    MemberCell* memberCell = static_cast<MemberCell*>(&member);
    if (memberCell == &ClassCell::memberClass())
        return true;
    if (!memberCell)
        return false;

    return m_members.find(memberCell) != m_members.end();
}

CellI& DataCell::member(CellI& member)
{
    MemberCell* memberCell = static_cast<MemberCell*>(&member);
    if (!memberCell)
        return emptyDataCell();
    auto findIt = m_members.find(memberCell);
    if (findIt == m_members.end())
        return emptyDataCell();

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

std::string DataCell::name() const
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

void DataCell::staticInit()
{
    static ClassCell emptyClass("Empty");
    s_emptyDataCell.reset(new DataCell(emptyClass));
}

DataCell& DataCell::emptyDataCell()
{
    return *s_emptyDataCell;
}

// ============================================================================
std::unique_ptr<ClassCell> ListItemCell::s_classCell;

MemberCell* ListItemCell::s_memberPrev  = nullptr;
MemberCell* ListItemCell::s_memberNext  = nullptr;
MemberCell* ListItemCell::s_memberValue = nullptr;

ListItemCell::ListItemCell()
{
}

bool ListItemCell::hasMember(CellI& member)
{
    if (&member == &ClassCell::memberClass() || &member == s_memberPrev || &member == s_memberNext || &member == s_memberValue) {
        return true;
    }

    return false;
}

CellI& ListItemCell::member(CellI& member)
{
    if (&member == &ClassCell::memberClass()) {
        return *s_classCell;
    }
    if (&member == s_memberPrev) {
        if (m_prev)
            return *m_prev;
        else
            return DataCell::emptyDataCell();
    }
    if (&member == s_memberNext) {
        if (m_next)
            return *m_next;
        else
            return DataCell::emptyDataCell();
    }
    if (&member == s_memberValue) {
        if (m_value)
            return *m_value;
        else
            return *m_value;
    }

    return DataCell::emptyDataCell();
}

ClassCell& ListItemCell::reflect()
{
    return *s_classCell;
}

std::string ListItemCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::string ListItemCell::name() const
{
    std::stringstream ss;
    if (m_value) {
        ss << "\"" << m_value->name() << "\"";
    }
    return ss.str();
}

CellI& ListItemCell::prev()
{
    return *m_prev;
}

void ListItemCell::prev(ListItemCell* p)
{
    m_prev = p;
}

CellI& ListItemCell::next()
{
    return *m_next;
}

void ListItemCell::next(ListItemCell* n)
{
    m_next = n;
}

CellI& ListItemCell::value()
{
    return *m_value;
}

void ListItemCell::value(CellI* v)
{
    m_value = v;
}

void ListItemCell::staticInit()
{
    s_classCell.reset(new ClassCell("ListItem"));

    s_memberPrev  = &s_classCell->createMember("prev", *s_classCell);
    s_memberNext  = &s_classCell->createMember("next", *s_classCell);
    s_memberValue = &s_classCell->createMember("value", *s_classCell); // TODO we need the T from the List<T> here somehow
}

void ListItemCell::staticInitMembers()
{
}

ClassCell& ListItemCell::classCell()
{
    return *s_classCell;
}

MemberCell& ListItemCell::memberPrev()
{
    return *s_memberPrev;
}
MemberCell& ListItemCell::memberNext()
{
    return *s_memberNext;
}

MemberCell& ListItemCell::memberValue()
{
    return *s_memberValue;
}

// ============================================================================
std::unique_ptr<ClassCell> ListCell::s_classCell;

MemberCell* ListCell::s_memberFirst = nullptr;
MemberCell* ListCell::s_memberLast  = nullptr;
MemberCell* ListCell::s_memberSize  = nullptr;

template <typename T>
ListCell::ListCell(const std::vector<T>& values)
{
    m_items.reserve(values.size());
    ListItemCell* prevListItemCell = nullptr;
    for (CellI* value : values) {
        auto& listItemCell = m_items.emplace_back();
        if (prevListItemCell == nullptr) {
            listItemCell.prev(nullptr);
        } else {
            listItemCell.prev(prevListItemCell);
            prevListItemCell->next(&listItemCell);
        }
        listItemCell.next(nullptr);
        listItemCell.value(value);
        prevListItemCell = &listItemCell;
    }
}

template <typename T>
ListCell::ListCell(std::map<std::string, T>& values)
{
    m_items.reserve(values.size());
    ListItemCell* prevListItemCell = nullptr;
    for (auto& valuePairs : values) {
        CellI* value       = valuePairs.second;
        auto& listItemCell = m_items.emplace_back();
        if (prevListItemCell == nullptr) {
            listItemCell.prev(nullptr);
        } else {
            listItemCell.prev(prevListItemCell);
            prevListItemCell->next(&listItemCell);
        }
        listItemCell.next(nullptr);
        listItemCell.value(value);
        prevListItemCell = &listItemCell;
    }
}

bool ListCell::hasMember(CellI& member)
{
    if (&member == s_memberFirst || &member == s_memberLast || &member == s_memberSize || &member == &ClassCell::memberClass()) {
        return true;
    }
    return false;
}

CellI& ListCell::member(CellI& member)
{
    if (&member == &ClassCell::memberClass()) {
        return *s_classCell;
    } else if (&member == s_memberFirst) {
        if (m_items.empty()) {
            return DataCell::emptyDataCell();
        }
        return m_items.front();
    } else if (&member == s_memberLast) {
        if (m_items.empty()) {
            return DataCell::emptyDataCell();
        }
        return m_items.back();
    } else if (&member == s_memberSize) {
        NumberCell* numberCell = new NumberCell((int)m_items.size());
        return *numberCell;
    }

    return DataCell::emptyDataCell();
}

ClassCell& ListCell::reflect()
{
    return *s_classCell;
}

std::string ListCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::string ListCell::name() const
{
    std::stringstream ss;
    ss << "List<>" << std::endl;

    return ss.str();
}

std::vector<ListItemCell>& ListCell::items()
{
    return m_items;
}

void ListCell::staticInit()
{
    s_classCell.reset(new ClassCell("List"));

    s_memberFirst = &s_classCell->createMember("first", ListItemCell::classCell());
    s_memberLast  = &s_classCell->createMember("last", ListItemCell::classCell());
}

void ListCell::staticInitMembers()
{
    s_memberSize = &s_classCell->createMember("size", NumberCell::classCell());
}

ClassCell& ListCell::classCell()
{
    return *s_classCell;
}

MemberCell& ListCell::memberFirst()
{
    return *s_memberFirst;
}

MemberCell& ListCell::memberLast()
{
    return *s_memberLast;
}

MemberCell& ListCell::memberSize()
{
    return *s_memberSize;
}

// ============================================================================
std::unique_ptr<ClassCell> DigitCells::s_digitClassCell;
std::vector<DataCell> DigitCells::s_digits;

void DigitCells::staticInit()
{
    s_digitClassCell.reset(new ClassCell("Digit"));
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        s_digits.push_back({ digitName, *s_digitClassCell });
    }
}

// ============================================================================
std::unique_ptr<ClassCell> NumberCell::s_classCell;
MemberCell* NumberCell::s_memberValue = nullptr;
MemberCell* NumberCell::s_memberSign  = nullptr;

NumberCell::NumberCell(int value) :
    m_value(value)
{
}

bool NumberCell::hasMember(CellI& member)
{
    if (&member == s_memberValue || &member == s_memberSign || &member == &ClassCell::memberClass()) {
        return true;
    }
    return false;
}

CellI& NumberCell::member(CellI& member)
{
    if (&member == &ClassCell::memberClass()) {
        return *s_classCell;
    } else if (&member == s_memberValue) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsListCell.reset(new ListCell(m_digits));
        }

        return *m_digitsListCell;
    } else {
        return DataCell::emptyDataCell();
    }
}

ClassCell& NumberCell::reflect()
{
    return *s_classCell;
}

std::string NumberCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::string NumberCell::name() const
{
    return std::to_string(m_value);
}

int NumberCell::value() const
{
    return m_value;
}

void NumberCell::staticInit()
{
    s_classCell.reset(new ClassCell("Number"));
    s_memberValue = &s_classCell->createMember("value", ListCell::classCell());
    s_memberSign  = &s_classCell->createMember("sign", NumberCell::classCell()); // TODO
}

ClassCell& NumberCell::classCell()
{
    return *s_classCell;
}

MemberCell& NumberCell::memberSign()
{
    return *s_memberSign;
}

MemberCell& NumberCell::memberValue()
{
    return *s_memberValue;
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
    std::reverse(m_digits.begin(), m_digits.end());
}

// ============================================================================
std::unique_ptr<ClassCell> UnicodeCells::s_unicodeClassCell;
std::map<char32_t, DataCell> UnicodeCells::s_characters;

void UnicodeCells::staticInit()
{
    s_unicodeClassCell.reset(new ClassCell("UnicodeCharacter"));
    createUnicodeCells(0x020, 0x07e);
    createUnicodeCells(0x080, 0x0ff);
    createUnicodeCells(0x100, 0x17f);
}

DataCell& UnicodeCells::unicodeValueToCell(char32_t utf32Char)
{
    auto unicodeCellIt = s_characters.find(utf32Char);
    if (unicodeCellIt != s_characters.end()) {
        return unicodeCellIt->second;
    } else {
        return DataCell::emptyDataCell();
    }
}

ClassCell& UnicodeCells::unicodeClassCell()
{
    return *s_unicodeClassCell;
}

void UnicodeCells::createUnicodeCells(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        s_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(characterName, *s_unicodeClassCell));
    }
}

// ============================================================================
std::unique_ptr<ClassCell> StringCell::s_classCell;
MemberCell* StringCell::s_memberCharacters = nullptr;

StringCell::StringCell(const std::string& str) :
    m_value(str)
{
}

bool StringCell::hasMember(CellI& member)
{
    if (&member == s_memberCharacters || &member == &ClassCell::memberClass()) {
        return true;
    }
    return false;
}

CellI& StringCell::member(CellI& member)
{
    if (&member == &ClassCell::memberClass()) {
        return *s_classCell;
    } else if (&member == s_memberCharacters) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersListCell.reset(new ListCell(m_characters));
        }

        return *m_charactersListCell;
    } else {
        return DataCell::emptyDataCell();
    }
}

ClassCell& StringCell::reflect()
{
    return *s_classCell;
}

std::string StringCell::printAs(CellPrinter& printer)
{
    return printer.print(*this);
}

std::string StringCell::name() const
{
    return m_value;
}

const std::string& StringCell::value() const
{
    return m_value;
}

void StringCell::staticInit()
{
    s_classCell.reset(new ClassCell("String"));
    s_memberCharacters = &s_classCell->createMember("value", ListCell::classCell());
}

ClassCell& StringCell::classCell()
{
    return *s_classCell;
}

MemberCell& StringCell::memberCharacters()
{
    return *s_memberCharacters;
}

void StringCell::calculateCharacters()
{
    utf8::iterator<const char*> valueIt(m_value.data(), m_value.data(), m_value.data() + m_value.size());
    utf8::iterator<const char*> valueEndIt(m_value.data() + m_value.size(), m_value.data(), m_value.data() + m_value.size());

    for (auto& it = valueIt; it != valueEndIt; ++valueIt) {
        char32_t unicodeValue = *it;
        m_characters.push_back(&UnicodeCells::unicodeValueToCell(unicodeValue));
    }
}

// ============================================================================
void StaticInitializations()
{
    ClassCell::staticInit();
    MemberCell::staticInit();
    DataCell::staticInit();
    DigitCells::staticInit();
    ListItemCell::staticInit();
    ListCell::staticInit();
    NumberCell::staticInit();
    UnicodeCells::staticInit();
    StringCell::staticInit();
    ClassCell::staticInitMembers();
    MemberCell::staticInitMembers();
    ListCell::staticInitMembers();
}

// ============================================================================
std::string CellValuePrinter::print(MemberCell& memberCell)
{
    std::stringstream ss;
    ss << memberCell.name() << ": " << memberCell.connectionClass().name();

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
        ss << memberI.first << ": " << memberI.second->connectionClass().name();
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(DataCell& dataCell)
{
    std::stringstream ss;
    if (!dataCell.name().empty()) {
        ss << dataCell.name() << ": ";
    }
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

std::string CellValuePrinter::print(ListItemCell& listItemCell)
{
    std::stringstream ss;
    ss << "[ ";
    if (!listItemCell.value().name().empty()) {
        ss << listItemCell.value().name() << " ";
    }
    ss << "]";

    return ss.str();
}

std::string CellValuePrinter::print(ListCell& listCell)
{
    std::stringstream ss;
    ss << "[";
    bool isFirst = true;
    for (auto& item : listCell.items()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ",";
        }
        ss << " " << item.value().name();
    }
    ss << " ]";

    return ss.str();
}

std::string CellValuePrinter::print(NumberCell& cell)
{
    std::stringstream ss;
    ss << "(Number) " << cell.value();

    return ss.str();
}

std::string CellValuePrinter::print(StringCell& cell)
{
    std::stringstream ss;
    ss << "(String) " << cell.value();

    return ss.str();
}

// ============================================================================
std::string CellStructPrinter::print(MemberCell& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(ClassCell& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(DataCell& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(ListItemCell& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(ListCell& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(NumberCell& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(StringCell& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::printImpl(CellI& cell)
{
    std::stringstream ss;
    ClassCell& classCell = cell.reflect();
    ss << "(" << classCell.name() << ") ID" << &cell << std::endl;
    for (auto& memberI : classCell.members()) {
        const std::string& memberName = memberI.first;
        MemberCell& memberCell = *memberI.second;

        if (!cell.hasMember(memberCell)) {
            continue;
        }
        CellValuePrinter valuePrinter;
        ClassCell& connectWith = static_cast<ClassCell&>(memberCell.member(MemberCell::memberConnectWith()));
        CellI& connectedNode = cell.member(memberCell);
        ss << "    +--(" << memberName << ")--> (" << connectWith.name() << ") ID" << &connectedNode << " // " << connectedNode.printAs(valuePrinter) << std::endl;
    }
    return ss.str();
}

} // namespace newcell
} // namespace synth