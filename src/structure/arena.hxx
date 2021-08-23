
#pragma once

#include <cassert>
#include <cstdint>
// __WORDSIZE
#include <cstdlib>
using std::realloc;
using std::free;
#include <climits>
// CHAR_BIT
#include <stdexcept>
using std::overflow_error;

#include <string>
using std::string;
using namespace std::string_literals;

#include <iostream>
using std::ostream;

#if __WORDSIZE == 64
#define MEM_UNIT std::uint_fast64_t
#else
#define MEM_UNIT std::uint_fast32_t
#endif

struct MemArena
{
    using mem_unit = MEM_UNIT;

    MemArena (size_t start_cap = 1024*1024);
    ~MemArena ();

    //MemArena           (MemArena&& ma2);
    //MemArena& operator=(MemArena&& ma2);

    mem_unit* alloc (size_t bytes); // alloc `bytes` bytes

private:
    mem_unit* arena;
    size_t sz, cap; // mem units in store

    void capacity (size_t min_cap);
};

struct MemoryOverflow : public overflow_error
{
    MemoryOverflow (const string& what_arg) : overflow_error(what_arg) {}
};
