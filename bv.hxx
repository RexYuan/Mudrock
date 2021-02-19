
#pragma once

#include <cassert>
#include <cstdint>
#include <climits>

#include <string>

#include <iostream>

using std::string;

//=================================================================================================
// Bit vector type
//
struct Bv
{
    Bv (size_t len); // construct 0s bv of length `len`
    Bv (string bs); // construct bv corresponding to `bs`
    ~Bv ();

    Bv (const Bv& bv2); // copy constructor
    Bv& operator=(const Bv& bv2); // copy assignment

    Bv (Bv&& bv2); // move constructor
    Bv& operator=(Bv&& bv2); // move assignment

    size_t len () const; // get length
    bool getter (size_t ind) const; // return value of ind
    void setter (size_t ind, bool val); // set ind to val
    void flipper (size_t ind); // flip value of ind

    void setter (string bs); // set to bs

    bool operator [] (size_t ind) const;

    friend bool operator == (const Bv& bv1, const Bv& bv2);
    friend bool operator != (const Bv& bv1, const Bv& bv2);
    friend bool operator <  (const Bv& bv1, const Bv& bv2);
    friend bool operator <= (const Bv& bv1, const Bv& bv2);

    Bv& operator &= (const Bv& bv2);
    Bv& operator |= (const Bv& bv2);
    Bv& operator ^= (const Bv& bv2);

    friend Bv operator ~ (Bv bv);
    friend Bv operator & (Bv bv1, Bv bv2);
    friend Bv operator | (Bv bv1, Bv bv2);
    friend Bv operator ^ (Bv bv1, Bv bv2);

    struct BitIterator;
    BitIterator begin() const;
    BitIterator end() const;

    string to_string () const; // pretty printing

    // data unit store
    using data_unit = std::uint_fast8_t;
    // make sure it's index-able
    static_assert(sizeof(size_t) >= sizeof(data_unit));

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

bool operator == (const Bv& bv1, const Bv& bv2);
bool operator != (const Bv& bv1, const Bv& bv2);
bool operator <  (const Bv& bv1, const Bv& bv2);
bool operator <= (const Bv& bv1, const Bv& bv2);

Bv operator ~ (Bv bv);
Bv operator & (Bv bv1, Bv bv2);
Bv operator | (Bv bv1, Bv bv2);
Bv operator ^ (Bv bv1, Bv bv2);

inline std::ostream& operator<<(std::ostream &out, const Bv& bv)
{
    out << bv.to_string();
    return out;
}