
#pragma once

#include <cassert>
#include <cstdint>

#include <string>

#include <iostream>

using std::string;

// Data unit store
//
// fastest type with at least 32 bits
using data_unit = std::uint_fast32_t;
// make sure it's index-able
static_assert(sizeof(size_t) >= sizeof(data_unit));

//=================================================================================================
// Bit vector type
//
struct Bv
{
    Bv (size_t len); // construct 0s bv of length `len`
    Bv (string bs); // construct bv corresponding to `bs`
    ~Bv ();

    size_t len () const; // get length
    bool getter (size_t ind) const; // return value of ind
    void setter (size_t ind, bool val); // set ind to val
    void flipper (size_t ind); // flip value of ind

    void setter (string bs); // set to bs

    struct BitIterator;
    BitIterator begin() const;
    BitIterator end() const;

    string to_string () const; // pretty printing

private:
    size_t length;
    data_unit* data;
};

//=================================================================================================
// Iterate over bits
//
struct Bv::BitIterator
{
    BitIterator (data_unit* ptr, size_t ind);

    BitIterator& operator++(); // prefix increment

    const bool operator*() const; // dereferencing returns a read-only bool
    void setbit (bool val);  // write with this method

    inline friend bool operator== (const BitIterator& a, const BitIterator& b)
        { return a.p == b.p && a.i == b.i; }
    inline friend bool operator!= (const BitIterator& a, const BitIterator& b)
        { return a.p != b.p || a.i != b.i; }

private:
    data_unit* p;
    size_t i;
    data_unit mask_ind;
};
inline Bv::BitIterator Bv::begin() const { return BitIterator(data, 0); }
inline Bv::BitIterator Bv::end() const { return BitIterator(data, length); } 

inline std::ostream& operator<<(std::ostream &out, const Bv& bv)
{
    out << bv.to_string();
    return out;
}
