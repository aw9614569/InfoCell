#pragma once

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

namespace synth {
namespace newcell {

class ClassCell;
class CellPrinter;
class CellI
{
public:
    virtual bool hasMember(CellI& member)             = 0;
    virtual CellI& member(CellI& member)              = 0;
    virtual ClassCell& reflect()                      = 0;
    virtual std::string printAs(CellPrinter& printer) = 0;
    virtual std::string name() const                  = 0;
};

class StringCell;
class MemberCell : public CellI
{
public:
    MemberCell(const std::string& name, ClassCell& classCell);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    static void staticInit();
    static void staticInitMembers();
    static MemberCell& memberConnectWith();
    static MemberCell& memberName();
    static MemberCell& memberSemantic();

    ClassCell& connectionClass();

protected:
    static std::unique_ptr<ClassCell> s_classCell;
    static MemberCell* s_memberConnectWith;
    static MemberCell* s_memberName;
    static MemberCell* s_memberSemantic;
    std::string m_name;
    std::unique_ptr<StringCell> m_classNameListCell;
    ClassCell& m_connectionClass; // the class of the connected cell
};

class ListCell;
class ClassCell : public CellI
{
public:
    explicit ClassCell(const std::string& name);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    static void staticInit();
    static void staticInitMembers();
    MemberCell& createMember(const std::string& name, ClassCell& classCell);
    void referenceMember(const std::string& name, MemberCell& memberCell);
    bool hasMember(const std::string& name) const;
    MemberCell& getMember(const std::string& name);
    std::map<std::string, MemberCell*>& members();
    static ClassCell& classCell();
    static MemberCell& memberClass();
    static MemberCell& memberMembers();
    static ClassCell& anyClassCell();

protected:
    static std::unique_ptr<ClassCell> s_classCell;
    static MemberCell* s_memberClass;
    static MemberCell* s_memberMembers;
    static std::unique_ptr<ClassCell> s_anyClassCell;
    std::map<std::string, MemberCell> m_memberCells;
    std::map<std::string, MemberCell*> m_memberRefs;
    std::string m_name;
    std::unique_ptr<ListCell> m_membersListCell;
};

class DataCell : public CellI
{
public:
    DataCell(ClassCell& classCell);
    DataCell(const std::string& name, ClassCell& classCell);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    std::map<MemberCell*, CellI*>& members();
    void connect(MemberCell& memberCell, CellI& value);

    static void staticInit();
    static DataCell& emptyDataCell();

protected:
    static std::unique_ptr<DataCell> s_emptyDataCell;
    std::string m_name;
    ClassCell& m_classCell;
    std::map<MemberCell*, CellI*> m_members;
};

class DigitCells
{
public:
    static void staticInit();
    static std::unique_ptr<ClassCell> s_digitClassCell;
    static std::vector<DataCell> s_digits;
};

class ListItemCell : public CellI
{
public:
    ListItemCell();

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    CellI& prev();
    void prev(ListItemCell* p);

    CellI& next();
    void next(ListItemCell* n);

    CellI& value();
    void value(CellI* v);

    static void staticInit();
    static void staticInitMembers();
    static ClassCell& classCell();
    static MemberCell& memberPrev();
    static MemberCell& memberNext();
    static MemberCell& memberValue();

protected:
    static std::unique_ptr<ClassCell> s_classCell;
    static MemberCell* s_memberPrev;
    static MemberCell* s_memberNext;
    static MemberCell* s_memberValue;

    CellI* m_prev = nullptr;
    CellI* m_next = nullptr;
    CellI* m_value = nullptr;
};

class ListCell : public CellI
{
public:
    template <typename T>
    ListCell(const std::vector<T>& values);

    template <typename T>
    ListCell(std::map<std::string, T>& values);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    std::vector<ListItemCell>& items();

    static void staticInit();
    static void staticInitMembers();

    static ClassCell& classCell();
    static MemberCell& memberFirst();
    static MemberCell& memberLast();
    static MemberCell& memberSize();

protected:
    static std::unique_ptr<ClassCell> s_classCell;
    static MemberCell* s_memberFirst;
    static MemberCell* s_memberLast;
    static MemberCell* s_memberSize;
    std::vector<ListItemCell> m_items;
};

class NumberCell : public CellI
{
public:
    NumberCell(int value);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    int value() const;

    static void staticInit();
    static ClassCell& classCell();
    static MemberCell& memberSign();
    static MemberCell& memberValue();

protected:
    void calculateDigits();

    static std::unique_ptr<ClassCell> s_classCell;
    static MemberCell* s_memberSign;
    static MemberCell* s_memberValue ;

    int m_value;
    std::vector<DataCell*> m_digits;
    std::unique_ptr<ListCell> m_digitsListCell;
};

class UnicodeCells
{
public:
    static void staticInit();
    static DataCell& unicodeValueToCell(char32_t utf32Char);
    static ClassCell& unicodeClassCell();

protected:
    static void createUnicodeCells(char32_t from, char32_t to);
    static std::unique_ptr<ClassCell> s_unicodeClassCell;
    static std::map<char32_t, DataCell> s_characters; // Unicode value to Cell
};

class StringCell : public CellI
{
public:
    StringCell(const std::string& str);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;
    std::string name() const override;

    const std::string& value() const;

    static void staticInit();
    static ClassCell& classCell();
    static MemberCell& memberCharacters();

protected:
    void calculateCharacters();

    static std::unique_ptr<ClassCell> s_classCell;
    static MemberCell* s_memberCharacters;

    std::string m_value;
    std::vector<DataCell*> m_characters;
    std::unique_ptr<ListCell> m_charactersListCell;
};

class CellPrinter
{
public:
    virtual std::string print(MemberCell& cell)   = 0;
    virtual std::string print(ClassCell& cell)    = 0;
    virtual std::string print(DataCell& cell)     = 0;
    virtual std::string print(ListItemCell& cell) = 0;
    virtual std::string print(ListCell& cell)     = 0;
    virtual std::string print(NumberCell& cell)   = 0;
    virtual std::string print(StringCell& cell)   = 0;
};

class CellValuePrinter : public CellPrinter
{
public:
    std::string print(MemberCell& cell) override;
    std::string print(ClassCell& cell) override;
    std::string print(DataCell& cell) override;
    std::string print(ListItemCell& cell) override;
    std::string print(ListCell& cell) override;
    std::string print(NumberCell& cell) override;
    std::string print(StringCell& cell) override;
};

class CellStructPrinter : public CellPrinter
{
public:
    std::string print(MemberCell& cell) override;
    std::string print(ClassCell& cell) override;
    std::string print(DataCell& cell) override;
    std::string print(ListItemCell& cell) override;
    std::string print(ListCell& cell) override;
    std::string print(NumberCell& cell) override;
    std::string print(StringCell& cell) override;

    std::string printImpl(CellI& cell);
};

void StaticInitializations();

} // namespace newcell
} // namespace synth