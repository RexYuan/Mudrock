
#pragma once

#include <cassert>
#include <cstdint>
// __WORDSIZE
#include <climits>
// CHAR_BIT

#include <string>
using std::string;
using namespace std::string_literals;

#include <memory>
using std::allocator_traits;
using std::pointer_traits;

#include <iostream>
using std::ostream;

#include "arena_alloc.hxx"

#if __WORDSIZE == 64
#define FAST_TYPE std::uint_fast64_t
#else
#define FAST_TYPE std::uint_fast32_t
#endif

// data unit store
using bv_data_unit = FAST_TYPE;
// make sure it's index-able
static_assert(sizeof(size_t) >= sizeof(bv_data_unit));

//=================================================================================================
// Bv factory
//
struct Bv;
using Bv_ptr = allocator_traits<ArenaAlloc<Bv>>::pointer;
Bv_ptr mkBv (const string& bs);
Bv_ptr mkBv (size_t len, unsigned i = 0);
Bv_ptr mkBv (const Bv_ptr bv2);

//=================================================================================================
// Bit vector type
//
struct Bv
{
    friend Bv_ptr mkBv (const string&);
    friend Bv_ptr mkBv (size_t, unsigned);
    friend Bv_ptr mkBv (const Bv_ptr);

    Bv () = delete;
    ~Bv () = default;
    Bv& operator=(const Bv& bv2);
    Bv           (Bv&& bv2) = delete;
    Bv& operator=(Bv&& bv2) = delete;

    size_t len  ()           const; // get length
    bool getter (size_t ind) const; // return value of ind

    void setter  (size_t ind, bool val); // set ind to val
    void flipper (size_t ind);           // flip value of ind

    void setter (const string& bs);  // set to bs
    void setter (unsigned i); // set to i

    bool operator [] (size_t ind) const;

    friend bool operator == (const Bv_ptr bv1, const Bv_ptr bv2);
    friend bool operator != (const Bv_ptr bv1, const Bv_ptr bv2);
    friend bool operator <  (const Bv_ptr bv1, const Bv_ptr bv2);
    friend bool operator <= (const Bv_ptr bv1, const Bv_ptr bv2);

    friend Bv_ptr operator ~ (const Bv_ptr bv);
    friend Bv_ptr operator & (const Bv_ptr bv1, const Bv_ptr bv2);
    friend Bv_ptr operator | (const Bv_ptr bv1, const Bv_ptr bv2);
    friend Bv_ptr operator ^ (const Bv_ptr bv1, const Bv_ptr bv2);

    struct BitIterator;
    BitIterator begin () const;
    BitIterator end   () const;

    string to_string        () const;
    string to_string_pretty () const;

private:
    Bv (size_t len); // construct 0s bv of length `len`
    Bv (const string& bs);  // construct bv corresponding to `bs`
    Bv (size_t len, unsigned i); // construct bv of length `len` corresponding to `i` (big-endian)
    Bv (const Bv& bv2);

    size_t length;
    bv_data_unit data[];

    inline void validate () { length > 0      ? assert(data)        : assert(data == nullptr);
                              data == nullptr ? assert(length == 0) : assert(length > 0); }
};

//=================================================================================================
// Iterate over bits
//
struct Bv::BitIterator
{
    BitIterator (bv_data_unit* ptr, size_t ind);

    BitIterator& operator++();    // prefix increment
    BitIterator  operator++(int); // postfix increment

    BitIterator& operator--();    // prefix decrement
    BitIterator  operator--(int); // postfix decrement

    const bool operator * () const; // dereferencing returns a read-only bool
    void setbit (bool val);         // write with this method

    friend bool operator == (const BitIterator& bit1, const BitIterator& bit2);
    friend bool operator != (const BitIterator& bit1, const BitIterator& bit2);

private:
    // stores native; allocating after instatiating an iterator might invalidate it from realloc
    bv_data_unit* p;
    size_t i;
    bv_data_unit mask_ind;
};

bool operator == (const Bv::BitIterator& bit1, const Bv::BitIterator& bit2);
bool operator != (const Bv::BitIterator& bit1, const Bv::BitIterator& bit2);

bool operator == (const Bv_ptr bv1, const Bv_ptr bv2);
bool operator != (const Bv_ptr bv1, const Bv_ptr bv2);
bool operator <  (const Bv_ptr bv1, const Bv_ptr bv2);
bool operator <= (const Bv_ptr bv1, const Bv_ptr bv2);

Bv_ptr operator ~ (const Bv_ptr bv);
Bv_ptr operator & (const Bv_ptr bv1, const Bv_ptr bv2);
Bv_ptr operator | (const Bv_ptr bv1, const Bv_ptr bv2);
Bv_ptr operator ^ (const Bv_ptr bv1, const Bv_ptr bv2);

inline ostream& operator << (ostream& out, const Bv_ptr bv) { out << bv->to_string(); return out; }
