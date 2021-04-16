
#pragma once

#include <cassert>

#include <vector>
using std::vector;
#include <array>
using std::array;
#include <string>
using std::string;
using std::to_string;
using namespace std::string_literals;
#include <concepts>
using std::same_as;
using std::convertible_to;
#include <type_traits>
using std::true_type;
using std::false_type;
using std::integral_constant;

#include <iostream>
using std::ostream;
#include <sstream>
using std::stringstream;
#include <iomanip>
using std::setw;
using std::setprecision;
using std::setfill;
#include <ios>
using std::fixed;
using std::left;
using std::right;

template <size_t N>
using Row_t = array<string, N>;

template <typename T>
struct is_Row : false_type {};
template <size_t N>
struct is_Row<Row_t<N>> : true_type {};

template <typename T>
struct Row_size {};
template <size_t N>
struct Row_size<Row_t<N>> : integral_constant<size_t, N> {};

template <typename T, typename... Ts>
concept can_mk_Row =
    (is_Row<T>::value) &&
    (sizeof...(Ts) == Row_size<T>::value) &&
    (convertible_to<Ts, string> && ...);

template <size_t N>
struct Table
{
    using Row = Row_t<N>;

    Table () = default;
   ~Table () = default;

    void set_header (Row r) { _header = r; }
    void push_entry (Row r) { _entries.push_back(r); }

    template <typename... Ts> requires can_mk_Row<Row, Ts...>
    void set_header (Ts... ss) { _header = Row{ss...}; }
    template <typename... Ts> requires can_mk_Row<Row, Ts...>
    void push_entry (Ts... ss) { _entries.push_back(Row{ss...}); }

    void push_entry () { _entries.push_back(Row{}); } // empty row

    Row         header  () const { return _header; }
    vector<Row> entries () const { return _entries; }

    string to_string () const;

private:
    const string sep = " "s;
    const size_t fields_size = N;
    Row         _header  = Row{};
    vector<Row> _entries = vector<Row>{};
};

namespace
{
    template <size_t N>
    inline const array<size_t, N> maxlens (const Table<N>& table)
    {
        array<size_t, N> tmp{};
        for (size_t field_num=0; field_num<N; field_num++)
            tmp[field_num] = table.header()[field_num].length();

        for (size_t Row_num=0; Row_num<table.entries().size(); Row_num++)
            for (size_t field_num=0; field_num<N; field_num++)
                if (table.entries()[Row_num][field_num].length() > tmp[field_num])
                    tmp[field_num] = table.entries()[Row_num][field_num].length();

        return tmp;
    }
}
template <size_t N>
string Table<N>::to_string () const
{
    auto field_wds = maxlens(*this);
    auto to_string = [=, *this](Row r) -> string
    {
        stringstream tmp;
        for (size_t field_num=0; field_num<N; field_num++)
            tmp << setw(field_wds[field_num]) << r[field_num] << sep;
        tmp << "\n";
        return tmp.str();
    };

    string ret = ""s;
    ret += to_string(header());
    for (Row r : entries())
        ret += to_string(r);

    return ret;
}

template <size_t N>
ostream& operator << (ostream& out, const Table<N>& t)
{
    out << t.to_string();
    return out;
}
