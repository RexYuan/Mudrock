
#pragma once

#include <cassert>
#include <cstdint>
// __WORDSIZE
#include <climits>
// CHAR_BIT

#include <string>
using std::string;
using namespace std::string_literals;

#include <iostream>
using std::ostream;

#if __WORDSIZE == 64
#define FAST_TYPE std::uint_fast64_t
#else
#define FAST_TYPE std::uint_fast32_t
#endif

//=================================================================================================
// Bit vector type
//
struct Bv
{
    Bv ();
    Bv (size_t len); // construct 0s bv of length `len`
    Bv (string bs);  // construct bv corresponding to `bs`
    Bv (size_t len, unsigned i); // construct bv of length `len` corresponding to `i` (big-endian)
    Bv (void* data_site, size_t len, unsigned i = 0); // construct at given address
    ~Bv ();

    Bv           (const Bv& bv2); // copy constructor
    Bv& operator=(const Bv& bv2); // copy assignment

    Bv           (Bv&& bv2); // move constructor
    Bv& operator=(Bv&& bv2); // move assignment

    size_t len  ()           const; // get length
    bool getter (size_t ind) const; // return value of ind

    void setter  (size_t ind, bool val); // set ind to val
    void flipper (size_t ind);           // flip value of ind

    void setter (string bs);  // set to bs
    void setter (unsigned i); // set to i

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
    BitIterator begin () const;
    BitIterator end   () const;

    explicit operator bool () const; // check if its degen

    string to_string        () const;
    string to_string_pretty () const;

    // data unit store
    using data_unit = FAST_TYPE;
    // make sure it's index-able
    static_assert(sizeof(size_t) >= sizeof(data_unit));

private:
    bool site_provided = false;
    size_t length;
    data_unit* data;

    inline void validate () { length > 0      ? assert(data)        : assert(data == nullptr);
                              data == nullptr ? assert(length == 0) : assert(length > 0); }
};

//=================================================================================================
// Iterate over bits
//
struct Bv::BitIterator
{
    BitIterator (data_unit* ptr, size_t ind);

    BitIterator& operator++();    // prefix increment
    BitIterator  operator++(int); // postfix increment

    BitIterator& operator--();    // prefix decrement
    BitIterator  operator--(int); // postfix decrement

    const bool operator * () const; // dereferencing returns a read-only bool
    void setbit (bool val);         // write with this method

    friend bool operator == (const BitIterator& bit1, const BitIterator& bit2);
    friend bool operator != (const BitIterator& bit1, const BitIterator& bit2);

private:
    data_unit* p;
    size_t i;
    data_unit mask_ind;
};

bool operator == (const Bv::BitIterator& bit1, const Bv::BitIterator& bit2);
bool operator != (const Bv::BitIterator& bit1, const Bv::BitIterator& bit2);

bool operator == (const Bv& bv1, const Bv& bv2);
bool operator != (const Bv& bv1, const Bv& bv2);
bool operator <  (const Bv& bv1, const Bv& bv2);
bool operator <= (const Bv& bv1, const Bv& bv2);

Bv operator ~ (Bv bv);
Bv operator & (Bv bv1, Bv bv2);
Bv operator | (Bv bv1, Bv bv2);
Bv operator ^ (Bv bv1, Bv bv2);

inline ostream& operator << (ostream& out, const Bv& bv) { out << bv.to_string(); return out; }
