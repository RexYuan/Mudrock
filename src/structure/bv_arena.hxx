
#pragma once

#include <cassert>
#include <climits>
// CHAR_BIT
#include <stdexcept>
using std::overflow_error;

#include <string>
using std::string;
using namespace std::string_literals;

#include <iostream>
using std::ostream;

#include "bv.hxx"
#include "old_arena.hxx"

struct BvArena
{
    BvArena () = default;
    ~BvArena () = default;

    //BvArena           (BvArena&& ba2);
    //BvArena& operator=(BvArena&& ba2);

    Bv* mkBv (size_t len, unsigned i = 0); // get a bv

private:
    MemArena ma;
};

struct SingletonBvArena
{
    static BvArena& Get ()
    {
        static BvArena ba{};
        return ba;
    }

    SingletonBvArena () {}
    SingletonBvArena           (const SingletonBvArena&) = delete;
    SingletonBvArena& operator=(const SingletonBvArena&) = delete;
    SingletonBvArena           (SingletonBvArena&&) = delete;
    SingletonBvArena& operator=(SingletonBvArena&&) = delete;
};
