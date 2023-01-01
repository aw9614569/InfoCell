#include "NewCells.h"

#include <format>

#pragma warning(disable : 4996)
#include <utf8.h>

namespace synth {
namespace newcell {

// ============================================================================
std::unique_ptr<Type> Slot::s_type;
Slot* Slot::s_slotSlotType = nullptr;
Slot* Slot::s_slotSlotName = nullptr;
Slot* Slot::s_slotSlotRole = nullptr;

Slot::Slot(const std::string& name, Type& classCell) :
    m_name(name), m_connectionClass(classCell)
{
}

bool Slot::hasSlot(CellI& slot)
{
    if (&slot == &Type::slotType() || &slot == s_slotSlotType || &slot == s_slotSlotName) {
        return true;
    }
    return false;
}

CellI& Slot::slot(CellI& slot)
{
    if (&slot == &Type::slotType()) {
        return *s_type;
    }
    if (&slot == s_slotSlotType) {
        return m_connectionClass;
    }
    if (&slot == s_slotSlotName) {
        if (!m_slotNameString) {
            m_slotNameString.reset(new String(m_name));
        }
        return *m_slotNameString;
    }
    if (&slot == s_slotSlotRole) {
        return *s_slotSlotRole;
    }

    return Object::emptyObject();
}

Type& Slot::type()
{
    return *s_type;
}

std::string Slot::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Slot::name() const
{
    return m_name;
}

void Slot::staticInit()
{
    s_type.reset(new Type("Member"));
}

void Slot::staticInitMembers()
{
    s_slotSlotType = &s_type->createSlot("slotType", Type::t());
    s_slotSlotName = &s_type->createSlot("slotName", String::t());
    s_slotSlotRole = &s_type->createSlot("slotRole", Type::anyType());
}

Type& Slot::t()
{
    return *s_type;
}

Slot& Slot::slotSlotName()
{
    return *s_slotSlotName;
}

Slot& Slot::slotSlotType()
{
    return *s_slotSlotType;
}

Slot& Slot::slotSlotRole()
{
    return *s_slotSlotRole;
}

Type& Slot::connectionClass()
{
    return m_connectionClass;
}

// ============================================================================
std::unique_ptr<Type> Type::s_type;
Slot* Type::s_slotType  = nullptr;
Slot* Type::s_slotSlots = nullptr;
std::unique_ptr<Type> Type::s_anyType;

Type::Type(const std::string& name) :
    m_name(name)
{
    if (!s_slotType) {
        return;
    }
    referenceSlot("type", *s_slotType);
}

bool Type::hasSlot(CellI& slot)
{
    if (&slot == &slotType() || &slot == s_slotSlots) {
        return true;
    }

    return false;
}

CellI& Type::slot(CellI& slot)
{
    if (&slot == &slotType()) {
        return *s_type;
    }
    if (&slot == s_slotSlots) {
        if (!m_slotsList)
            m_slotsList.reset(new List(m_slotRefs));

        return *m_slotsList;
    }

    return Object::emptyObject();
}

Type& Type::type()
{
    return *s_type;
}

std::string Type::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Type::name() const
{
    return m_name;
}

void Type::staticInit()
{
    s_type.reset(new Type("Type"));
    s_slotType = &s_type->createSlot("type", *s_type);
    s_type->referenceSlot("type", *s_slotType);
    s_anyType.reset(new Type("AnyType"));
}

void Type::staticInitMembers()
{
    static Type listItemForSlot("ListItem<Slot>");
    listItemForSlot.createSlot("prev", listItemForSlot);
    listItemForSlot.createSlot("next", listItemForSlot);
    listItemForSlot.createSlot("value", Slot::t());

    static Type listOfSlots("List<Slot>");
    listOfSlots.createSlot("first", listItemForSlot);
    listOfSlots.createSlot("last", listItemForSlot);
    listOfSlots.createSlot("size", Number::t());

    s_slotSlots = &s_type->createSlot("slots", listOfSlots);
}

Slot& Type::createSlot(const std::string& name, Type& classCell)
{
    auto slotIt = m_slotRefs.find(name);
    if (slotIt != m_slotRefs.end()) {
        if (&slotIt->second->connectionClass() != &classCell) {
            throw "Member name already registered with an other class";
        }
        return *slotIt->second;
    } else {
        auto it = m_slots.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(name),
                                  std::forward_as_tuple(name, classCell));

        m_slotRefs[name] = &it.first->second;

        return it.first->second;
    }
}

void Type::referenceSlot(const std::string& name, Slot& slotCell)
{
    m_slotRefs[name] = &slotCell;
}

bool Type::hasSlot(const std::string& name) const
{
    return m_slotRefs.find(name) != m_slotRefs.end();
}

Slot& Type::getSlot(const std::string& name)
{
    auto findIt = m_slotRefs.find(name);
    if (findIt == m_slotRefs.end())
        throw "emptyMember";

    return *findIt->second;
}

std::map<std::string, Slot*>& Type::slots()
{
    return m_slotRefs;
}

Type& Type::t()
{
    return *s_type;
}

Slot& Type::slotType()
{
    return *s_slotType;
}

Slot& Type::slotSlots()
{
    return *s_slotSlots;
}

Type& Type::anyType()
{
    return *s_anyType;
}

// ============================================================================
std::unique_ptr<Object> Object::s_emptyObject;

Object::Object(Type& classCell) :
    Object("", classCell)
{
}

Object::Object(const std::string& name, Type& classCell) :
    m_name(name), m_type(classCell)
{
    m_slots[&Type::slotType()] = &classCell;
}

bool Object::hasSlot(CellI& slot)
{
    Slot* slotCell = static_cast<Slot*>(&slot);
    if (slotCell == &Type::slotType())
        return true;
    if (!slotCell)
        return false;

    return m_slots.find(slotCell) != m_slots.end();
}

CellI& Object::slot(CellI& slot)
{
    Slot* slotCell = static_cast<Slot*>(&slot);
    if (!slotCell)
        return emptyObject();
    auto findIt = m_slots.find(slotCell);
    if (findIt == m_slots.end())
        return emptyObject();

    return *findIt->second;
}

Type& Object::type()
{
    return m_type;
}

std::string Object::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Object::name() const
{
    return m_name;
}

std::map<Slot*, CellI*>& Object::slots()
{
    return m_slots;
}

void Object::connect(Slot& slotCell, CellI& value)
{
    m_slots[&slotCell] = &value;
}

void Object::staticInit()
{
    static Type emptyClass("Empty");
    s_emptyObject.reset(new Object(emptyClass));
}

Object& Object::emptyObject()
{
    return *s_emptyObject;
}

// ============================================================================
ListItem::ListItem(Type& t) :
    m_type(t)
{
    m_slotPrev  = &t.getSlot("prev");
    m_slotNext  = &t.getSlot("next");
    m_slotValue = &t.getSlot("value");
}

bool ListItem::hasSlot(CellI& slot)
{
    if (&slot == &Type::slotType() || &slot == m_slotPrev || &slot == m_slotNext || &slot == m_slotValue) {
        return true;
    }

    return false;
}

CellI& ListItem::slot(CellI& slot)
{
    if (&slot == &Type::slotType()) {
        return m_type;
    }
    if (&slot == m_slotPrev) {
        if (m_prev)
            return *m_prev;
        else
            return Object::emptyObject();
    }
    if (&slot == m_slotNext) {
        if (m_next)
            return *m_next;
        else
            return Object::emptyObject();
    }
    if (&slot == m_slotValue) {
        if (m_value)
            return *m_value;
        else
            return *m_value;
    }

    return Object::emptyObject();
}

Type& ListItem::type()
{
    return m_type;
}

std::string ListItem::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string ListItem::name() const
{
    std::stringstream ss;
    if (m_value) {
        ss << "\"" << m_value->name() << "\"";
    }
    return ss.str();
}

CellI& ListItem::prev()
{
    return *m_prev;
}

void ListItem::prev(ListItem* p)
{
    m_prev = p;
}

CellI& ListItem::next()
{
    return *m_next;
}

void ListItem::next(ListItem* n)
{
    m_next = n;
}

CellI& ListItem::value()
{
    return *m_value;
}

void ListItem::value(CellI* v)
{
    m_value = v;
}

Slot& ListItem::slotPrev()
{
    return *m_slotPrev;
}
Slot& ListItem::slotNext()
{
    return *m_slotNext;
}

Slot& ListItem::slotValue()
{
    return *m_slotValue;
}

// ============================================================================
template <typename T>
List::List(const std::vector<T>& values) :
    m_listType("List<T>"),
    m_itemType("ListItem<T>")
{
    m_slotFirst = &m_listType.createSlot("first", m_itemType);
    m_slotLast  = &m_listType.createSlot("last", m_itemType);
    m_slotSize  = &m_listType.createSlot("size", Number::t());

    Type& valueType = values.front()->type();
    m_itemType.createSlot("prev", m_itemType);
    m_itemType.createSlot("next", m_itemType);
    m_itemType.createSlot("value", valueType);

    m_items.reserve(values.size());
    ListItem* prevListItem = nullptr;
    for (CellI* value : values) {
        auto& listItem = m_items.emplace_back(m_itemType);
        if (prevListItem == nullptr) {
            listItem.prev(nullptr);
        } else {
            listItem.prev(prevListItem);
            prevListItem->next(&listItem);
        }
        listItem.next(nullptr);
        listItem.value(value);
        prevListItem = &listItem;
    }
}

template <typename T>
List::List(std::map<std::string, T>& values) :
    m_listType("List<T>"),
    m_itemType("ListItem<T>")
{
    m_slotFirst = &m_listType.createSlot("first", m_itemType);
    m_slotLast  = &m_listType.createSlot("last", m_itemType);
    m_slotSize  = &m_listType.createSlot("size", Number::t());

    Type& valueType = (*values.begin()).second->type();
    m_itemType.createSlot("prev", m_itemType);
    m_itemType.createSlot("next", m_itemType);
    m_itemType.createSlot("value", valueType);

    m_items.reserve(values.size());
    ListItem* prevListItem = nullptr;
    for (auto& valuePairs : values) {
        CellI* value       = valuePairs.second;
        auto& listItemCell = m_items.emplace_back(m_itemType);
        if (prevListItem == nullptr) {
            listItemCell.prev(nullptr);
        } else {
            listItemCell.prev(prevListItem);
            prevListItem->next(&listItemCell);
        }
        listItemCell.next(nullptr);
        listItemCell.value(value);
        prevListItem = &listItemCell;
    }
}

bool List::hasSlot(CellI& slot)
{
    if (&slot == m_slotFirst || &slot == m_slotLast || &slot == m_slotSize || &slot == &Type::slotType()) {
        return true;
    }
    return false;
}

CellI& List::slot(CellI& slot)
{
    if (&slot == &Type::slotType()) {
        return m_listType;
    } else if (&slot == m_slotFirst) {
        if (m_items.empty()) {
            return Object::emptyObject();
        }
        return m_items.front();
    } else if (&slot == m_slotLast) {
        if (m_items.empty()) {
            return Object::emptyObject();
        }
        return m_items.back();
    } else if (&slot == m_slotSize) {
        Number* number = new Number((int)m_items.size());
        return *number;
    }

    return Object::emptyObject();
}

Type& List::type()
{
    return m_listType;
}

std::string List::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string List::name() const
{
    std::stringstream ss;
    ss << "List<>" << std::endl;

    return ss.str();
}

std::vector<ListItem>& List::items()
{
    return m_items;
}

Slot& List::slotFirst()
{
    return *m_slotFirst;
}

Slot& List::slotLast()
{
    return *m_slotLast;
}

Slot& List::slotSize()
{
    return *m_slotSize;
}

// ============================================================================
std::unique_ptr<Type> Digits::s_digitClassCell;
std::vector<Object> Digits::s_digits;

void Digits::staticInit()
{
    s_digitClassCell.reset(new Type("Digit"));
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        s_digits.push_back({ digitName, *s_digitClassCell });
    }
}

// ============================================================================
std::unique_ptr<Type> Number::s_type;
Slot* Number::s_slotValue = nullptr;
Slot* Number::s_slotSign  = nullptr;

Number::Number(int value) :
    m_value(value)
{
}

bool Number::hasSlot(CellI& slot)
{
    if (&slot == s_slotValue || &slot == s_slotSign || &slot == &Type::slotType()) {
        return true;
    }
    return false;
}

CellI& Number::slot(CellI& slot)
{
    if (&slot == &Type::slotType()) {
        return *s_type;
    } else if (&slot == s_slotValue) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(m_digits));
        }

        return *m_digitsList;
    } else {
        return Object::emptyObject();
    }
}

Type& Number::type()
{
    return *s_type;
}

std::string Number::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Number::name() const
{
    return std::to_string(m_value);
}

int Number::value() const
{
    return m_value;
}

void Number::staticInit()
{
    s_type.reset(new Type("Number"));
//    s_slotValue = &s_type->createSlot("value", List::t()); // TODO
    s_slotValue = &s_type->createSlot("value", Type::anyType());
    s_slotSign  = &s_type->createSlot("sign", Number::t()); // TODO
}

Type& Number::t()
{
    return *s_type;
}

Slot& Number::slotSign()
{
    return *s_slotSign;
}

Slot& Number::slotValue()
{
    return *s_slotValue;
}

void Number::calculateDigits()
{
    if (m_value == 0) {
        m_digits.push_back(&Digits::s_digits[0]);
        return;
    }
    int value = m_value;
    while (value) {
        m_digits.push_back(&Digits::s_digits[value % 10]);
        value /= 10;
    }
    std::reverse(m_digits.begin(), m_digits.end());
}

// ============================================================================
std::map<int, Number> Numbers::s_numbers;

Number& Numbers::get(int number)
{
    auto numberIt = s_numbers.find(number);
    if (numberIt != s_numbers.end()) {
        return numberIt->second;
    } else {
        auto it = s_numbers.emplace(number, number);
        return it.first->second;
    }
}

// ============================================================================
std::unique_ptr<Type> UnicodeCells::s_unicodeClassCell;
std::map<char32_t, Object> UnicodeCells::s_characters;

void UnicodeCells::staticInit()
{
    s_unicodeClassCell.reset(new Type("UnicodeCharacter"));
    createUnicodeCells(0x020, 0x07e);
    createUnicodeCells(0x080, 0x0ff);
    createUnicodeCells(0x100, 0x17f);
}

Object& UnicodeCells::unicodeValueToCell(char32_t utf32Char)
{
    auto unicodeCellIt = s_characters.find(utf32Char);
    if (unicodeCellIt != s_characters.end()) {
        return unicodeCellIt->second;
    } else {
        return Object::emptyObject();
    }
}

Type& UnicodeCells::unicodeTypeCell()
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
std::unique_ptr<Type> String::s_type;
Slot* String::s_slotCharacters = nullptr;

String::String(const std::string& str) :
    m_value(str)
{
}

bool String::hasSlot(CellI& slot)
{
    if (&slot == s_slotCharacters || &slot == &Type::slotType()) {
        return true;
    }
    return false;
}

CellI& String::slot(CellI& slot)
{
    if (&slot == &Type::slotType()) {
        return *s_type;
    } else if (&slot == s_slotCharacters) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersList.reset(new List(m_characters));
        }

        return *m_charactersList;
    } else {
        return Object::emptyObject();
    }
}

Type& String::type()
{
    return *s_type;
}

std::string String::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string String::name() const
{
    return m_value;
}

const std::string& String::value() const
{
    return m_value;
}

void String::staticInit()
{
    s_type.reset(new Type("String"));
//    s_slotCharacters = &s_type->createSlot("value", List::t());
    s_slotCharacters = &s_type->createSlot("value", Type::anyType()); // TODO
}

Type& String::t()
{
    return *s_type;
}

Slot& String::slotCharacters()
{
    return *s_slotCharacters;
}

void String::calculateCharacters()
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
    Type::staticInit();
    Slot::staticInit();
    Object::staticInit();
    Digits::staticInit();
    Number::staticInit();
    UnicodeCells::staticInit();
    String::staticInit();
    Type::staticInitMembers();
    Slot::staticInitMembers();
}

// ============================================================================
std::string CellValuePrinter::print(Slot& slotCell)
{
    std::stringstream ss;
    ss << slotCell.name() << ": " << slotCell.connectionClass().name();

    return ss.str();
}

std::string CellValuePrinter::print(Type& classCell)
{
    std::stringstream ss;
    ss << "class " << classCell.name() << " { ";
    bool isFirst = true;
    for (auto& slotI : classCell.slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << slotI.first << ": " << slotI.second->connectionClass().name();
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(Object& dataCell)
{
    std::stringstream ss;
    if (!dataCell.name().empty()) {
        ss << dataCell.name() << ": ";
    }
    ss << dataCell.type().name() << " { ";
    bool isFirst = true;
    for (auto& slotI : dataCell.slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << "." << slotI.first->name() << ": " << slotI.second->printAs(*this);
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(ListItem& listItemCell)
{
    std::stringstream ss;
    ss << "[ ";
    if (!listItemCell.value().name().empty()) {
        ss << listItemCell.value().name() << " ";
    }
    ss << "]";

    return ss.str();
}

std::string CellValuePrinter::print(List& List)
{
    std::stringstream ss;
    ss << "[";
    bool isFirst = true;
    for (auto& item : List.items()) {
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

std::string CellValuePrinter::print(Number& cell)
{
    std::stringstream ss;
    ss << "(Number) " << cell.value();

    return ss.str();
}

std::string CellValuePrinter::print(String& cell)
{
    std::stringstream ss;
    ss << "(String) " << cell.value();

    return ss.str();
}

// ============================================================================
std::string CellStructPrinter::print(Slot& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(Type& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(Object& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(ListItem& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(List& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(Number& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(String& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::printImpl(CellI& cell)
{
    std::stringstream ss;
    Type& classCell = cell.type();
    ss << "(" << classCell.name() << ") ID" << &cell << std::endl;
    for (auto& slotI : classCell.slots()) {
        const std::string& slotSlotName = slotI.first;
        Slot& slot                      = *slotI.second;

        if (!cell.hasSlot(slot)) {
            continue;
        }
        CellValuePrinter valuePrinter;
        Type& slotType       = static_cast<Type&>(slot.slot(Slot::slotSlotType()));
        CellI& connectedCell = cell.slot(slot);
        ss << "    +--(" << slotSlotName << ")--> (" << slotType.name() << ") ID" << &connectedCell << " // " << connectedCell.printAs(valuePrinter) << std::endl;
    }
    return ss.str();
}

} // namespace newcell
} // namespace synth