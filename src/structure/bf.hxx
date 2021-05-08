
#pragma once

#include <cassert>

#include <vector>
using std::vector;
#include <string>
using std::string;
using std::to_string;
using namespace std::string_literals;
#include <map>
using std::map;

#include <concepts>
using std::same_as;
#include <iostream>
using std::ostream;
#include <variant>
using std::variant;
using std::monostate;
using std::get;
using std::holds_alternative;
#include <memory>
using std::shared_ptr;
using std::make_shared;
#include <stdexcept>
using std::logic_error;

// boolean connectives
enum class Conn {Top, Bot, Base, Not, And, Or};

//=================================================================================================
// Boolean formula type
//
struct Bf;
using Bf_ptr = shared_ptr<Bf>;
template <typename T>     concept is_Bf_ptr   = same_as<T, Bf_ptr>;
template <typename... Ts> concept are_Bf_ptrs = (is_Bf_ptr<Ts> && ...);
struct Bf
{
    //=============================================================================================
    // Pointer manipulation helpers. Use these as constructors
    //
    friend Bf_ptr v    (bool b);                 // return bool constant valued `b`
    friend Bf_ptr v    (int i);                  // return variable numbered `i`
    friend Bf_ptr neg  (Bf_ptr bf);              // return `not bf`
    friend Bf_ptr conj (Bf_ptr bf1, Bf_ptr bf2); // return `bf1 and bf2`
    friend Bf_ptr disj (Bf_ptr bf1, Bf_ptr bf2); // return `bf1 or bf2`

    // substitute var space
    friend Bf_ptr subst (const Bf_ptr& bf, const map<int,int>& to);

    const bool            get_bool () const;
    const int             get_int  () const;
    const Bf_ptr&         get_sub  () const;
    const vector<Bf_ptr>& get_subs () const;

    void push_sub (Bf_ptr bf);

    string to_string ();

//DEBUG: the following cannot be private since that'd break smart pointers
//private:
    Conn t;
    variant<monostate, int, Bf_ptr, vector<Bf_ptr>> sub;

    //=============================================================================================
    // Private constructors. Do not call directly
    //
    // construct bool constant valued `b`
    inline Bf (bool b) : sub {monostate{}} { if (b) t=Conn::Top; else t=Conn::Bot; }
    // construct variable numbered `i`
    inline Bf (int i) : t {Conn::Base}, sub {int{i}} {}
    // construct unary formula node of type `c` containing `bf`
    inline Bf (Conn c, Bf_ptr bf) : t {c}, sub {Bf_ptr{bf}} {}
    // construct n-ary formula node of type `c` containing `bfs`
    template <typename... Ts> requires are_Bf_ptrs<Ts...>
    inline Bf (Conn c, Ts... bfs) : t {c}, sub {vector<Bf_ptr>{bfs...}} {}
    // construct uninitialized formula node of type `c`
    inline Bf (Conn c) : t {c}
    {
        switch (t)
        {
        case Conn::Top: [[fallthrough]];
        case Conn::Bot: sub = monostate{}; break;
        case Conn::Base: sub = int{}; break;
        case Conn::Not: sub = Bf_ptr{}; break;
        case Conn::And: [[fallthrough]];
        case Conn::Or: sub = vector<Bf_ptr>{}; break;
        }
    }
};

Bf_ptr v    (bool b);
Bf_ptr v    (int i);
Bf_ptr neg  (Bf_ptr bf);
Bf_ptr conj (Bf_ptr bf1, Bf_ptr bf2);
Bf_ptr disj (Bf_ptr bf1, Bf_ptr bf2);

inline Bf_ptr operator ~  (const Bf_ptr& bf)                     { return neg(bf); }
inline Bf_ptr operator &  (const Bf_ptr& bf1, const Bf_ptr& bf2) { return conj(bf1, bf2); }
inline Bf_ptr operator |  (const Bf_ptr& bf1, const Bf_ptr& bf2) { return disj(bf1, bf2); }
inline Bf_ptr operator |= (const Bf_ptr& bf1, const Bf_ptr& bf2) { return ~bf1 | bf2; }
inline Bf_ptr operator == (const Bf_ptr& bf1, const Bf_ptr& bf2) { return (~bf1 | bf2) & ( bf1 | ~bf2); }
inline Bf_ptr operator != (const Bf_ptr& bf1, const Bf_ptr& bf2) { return ( bf1 | bf2) & (~bf1 | ~bf2); }

inline ostream& operator << (ostream& out, const Bf_ptr& bf) { out << bf->to_string(); return out; }

struct InvalidBfConn : public logic_error
{
    InvalidBfConn (const string& what_arg) : logic_error(what_arg) {}
};
