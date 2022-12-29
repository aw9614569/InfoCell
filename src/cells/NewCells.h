#pragma once

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <span>
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
};

class MemberCell : public CellI
{
public:
    MemberCell(const std::string& name, ClassCell& classCell);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;

    static void staticInit();
    const std::string& name() const;
    ClassCell& classCell();

protected:
    static std::unique_ptr<ClassCell> s_classCell;
    std::string m_name;
    ClassCell& m_classCell; // the class of the connected cell
};

class ClassCell : public CellI
{
public:
    explicit ClassCell(const std::string& name);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;

    static void staticInit();
    const std::string& name() const;
    MemberCell& createMember(const std::string& name, ClassCell& classCell);
    void referenceMember(const std::string& name, MemberCell& memberCell);
    bool hasMember(const std::string& name) const;
    MemberCell& getMember(const std::string& name);
    std::map<std::string, MemberCell*>& members();
    static ClassCell& classCell();
    static MemberCell& classMember();

protected:
    static std::unique_ptr<ClassCell> s_classCell;
    static std::unique_ptr<MemberCell> s_classMember;
    std::map<std::string, MemberCell> m_memberCells;
    std::map<std::string, MemberCell*> m_memberRefs;
    std::string m_name;
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

    const std::string& name() const;
    std::map<MemberCell*, CellI*>& members();
    void connect(MemberCell& memberCell, CellI& value);

    static DataCell emptyDataCell;

protected:
    std::string m_name;
    ClassCell& m_classCell;
    std::map<MemberCell*, CellI*> m_members;
};

class DigitCells
{
public:
    static void staticInit();
    static ClassCell s_digitClassCell;
    static std::array<DataCell, 10> s_digits;
};


class ListCell : public CellI
{
public:
    template<typename T>
    ListCell(std::span<T> values);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;

    std::vector<DataCell>& values();

    static void staticInit();
    static ClassCell& classCell();

protected:
    static ClassCell s_classCell;
    static ClassCell s_listItemClassCell;
    static MemberCell* s_listItemPrevMember;
    static MemberCell* s_listItemNextMember;
    static MemberCell* s_listItemValueMember;
    static MemberCell* s_firstMember;
    static MemberCell* s_lastMember;
    static MemberCell* s_sizeMember;
    std::vector<DataCell> m_values;
};

class NumberCell : public CellI
{
public:
    NumberCell(int value);

    bool hasMember(CellI& member) override;
    CellI& member(CellI& member) override;
    ClassCell& reflect() override;
    std::string printAs(CellPrinter& printer) override;

    int value() const;

    static void staticInit();
    static ClassCell& classCell();

protected:
    void calculateDigits();

    static ClassCell s_classCell;
    static MemberCell* s_valueMember;
    static MemberCell* s_signMember;

    int m_value;
    std::vector<DataCell*> m_digits;
    std::unique_ptr<ListCell> m_digitsListCell;
};

class CellPrinter
{
public:
    virtual std::string print(MemberCell& cell) = 0;
    virtual std::string print(ClassCell& cell) = 0;
    virtual std::string print(DataCell& cell) = 0;
    virtual std::string print(ListCell& cell) = 0;
    virtual std::string print(NumberCell& cell) = 0;
};

class CellValuePrinter : public CellPrinter
{
public:
    std::string print(MemberCell& cell) override;
    std::string print(ClassCell& cell) override;
    std::string print(DataCell& cell) override;
    std::string print(ListCell& cell) override;
    std::string print(NumberCell& cell) override;
};

class CellStructPrinter : public CellPrinter
{
public:
    std::string print(MemberCell& cell) override;
    std::string print(ClassCell& cell) override;
    std::string print(DataCell& cell) override;
    std::string print(ListCell& cell) override;
    std::string print(NumberCell& cell) override;

    std::string printImpl(CellI& cell);
};

void StaticInitializations();

} // namespace newcell
} // namespace synth