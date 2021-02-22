
#pragma once

#include <cassert>

#include <vector>
#include <string>

#include <iostream>
#include <variant>
#include <memory>

#include "minisat.hxx"

using std::vector;
using std::string;
using std::shared_ptr; using std::make_shared;
using std::variant; using std::monostate; using std::get; using std::holds_alternative;
using std::same_as;

// boolean connectives
enum class Conn {Top, Bot, Base, Not, And, Or};

//=================================================================================================
// Boolean formula type
//
struct Bf;
using Bf_ptr = shared_ptr<Bf>;
template <typename T> concept is_Bf_ptr = same_as<T, Bf_ptr>;
template <typename... Ts> concept are_Bf_ptrs = (is_Bf_ptr<Ts> && ...);
struct Bf
{
    //=============================================================================================
    // Pointer manipulation helpers. Use these as constructors
    //
    friend Bf_ptr v (bool b); // return bool constant valued `b`
    friend Bf_ptr v (int i); // return variable numbered `i`
    friend Bf_ptr neg (Bf_ptr bf); // return `not bf`
    friend Bf_ptr conj (Bf_ptr bf1, Bf_ptr bf2); // return `bf1 and bf2`
    friend Bf_ptr disj (Bf_ptr bf1, Bf_ptr bf2); // return `bf1 or bf2`

    bool get_bool ();
    int get_int ();
    Bf_ptr get_sub ();
    vector<Bf_ptr> get_subs ();

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

Bf_ptr v (bool b);
Bf_ptr v (int i);
Bf_ptr neg (Bf_ptr bf);
Bf_ptr conj (Bf_ptr bf1, Bf_ptr bf2);
Bf_ptr disj (Bf_ptr bf1, Bf_ptr bf2);

inline Bf_ptr operator~(Bf_ptr bf) { return neg(bf); }
inline Bf_ptr operator&(Bf_ptr bf1, Bf_ptr bf2) { return conj(bf1,bf2); }
inline Bf_ptr operator|(Bf_ptr bf1, Bf_ptr bf2) { return disj(bf1,bf2); }
inline Bf_ptr operator|=(Bf_ptr bf1, Bf_ptr bf2) { return ~bf1 | bf2; }
inline Bf_ptr operator==(Bf_ptr bf1, Bf_ptr bf2) { return (~bf1 | bf2) & ( bf1 | ~bf2); }
inline Bf_ptr operator!=(Bf_ptr bf1, Bf_ptr bf2) { return ( bf1 | bf2) & (~bf1 | ~bf2); }

inline std::ostream& operator<<(std::ostream &out, const Bf_ptr& bf)
{
    out << bf->to_string();
    return out;
}
