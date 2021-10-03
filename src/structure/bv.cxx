
#include "bv.hxx"

//=================================================================================================
// Calculate bit-byte positions
//
// local only functions. not to be used outside this file
namespace
{
    // how many bits a byte contains
    inline consteval const size_t bits_in_bytes (size_t bytes) { return bytes*CHAR_BIT; }

    // how many bits a data unit contains
    inline constinit const size_t data_unit_bits = bits_in_bytes(sizeof(bv_data_unit));

    // mask of 1000...
    inline constinit const bv_data_unit one_zeros_mask = 1LL << (data_unit_bits-1U);

    // mask of 1111...
    inline constinit const bv_data_unit ones_mask = ~0LL;

    // how many units are needed to store `bits` bits
    inline constexpr const size_t bits_in_units (size_t bits)
    {
        assert (bits > 0U);
        return (bits+data_unit_bits-1U) / data_unit_bits;
    }

    // which unit is `index`th bit stored
    inline constexpr const size_t get_index_prefix (size_t index)
    {
        return index / data_unit_bits;
    }

    // which bit is `index`th bit stored in its unit
    inline constexpr const size_t get_index_suffix (size_t index)
    {
        return index % data_unit_bits;
    }

    // mask for extracting `index`th bit from its unit
    inline constexpr const size_t get_index_mask (size_t index)
    {
        assert (0U <= index && index <= data_unit_bits);
        return one_zeros_mask >> index;
    }

    // mask for removing the rightmost n bits
    inline constexpr const size_t get_right_zeros_mask (size_t len)
    {
        assert (0U <= len && len <= data_unit_bits);
        return ones_mask << len;
    }

    // reset data trail to zeros
    inline constexpr bv_data_unit reset_data_trail (bv_data_unit u, size_t total_bit_len)
    {
        return u & get_right_zeros_mask(data_unit_bits-total_bit_len%data_unit_bits);
    }
}

//=================================================================================================
// Bv factory
//
using byte_alloc = ArenaAlloc<unsigned char>;
using byte_alloc_traits = allocator_traits<byte_alloc>;
using byte_ptr = byte_alloc_traits::pointer;
using byte_ptr_traits = std::pointer_traits<byte_ptr>;
Bv_ptr mkBv (const string& bs)
{
    static byte_alloc ba;
    size_t needed = sizeof(Bv) + sizeof(bv_data_unit)*bits_in_units(bs.length());
    byte_ptr site = byte_alloc_traits::allocate(ba, needed);
    new (site.get()) Bv{bs};
    return byte_ptr_traits::rebind<Bv>{byte_ptr_traits::rebind<void>{site}};
}

Bv_ptr mkBv (size_t len, unsigned i)
{
    static byte_alloc ba;
    size_t needed = sizeof(Bv) + sizeof(bv_data_unit)*bits_in_units(len);
    byte_ptr site = byte_alloc_traits::allocate(ba, needed);
    new (site.get()) Bv{len, i};
    return byte_ptr_traits::rebind<Bv>{byte_ptr_traits::rebind<void>{site}};
}

Bv_ptr mkBv (const Bv_ptr bv2)
{
    static byte_alloc ba;
    size_t needed = sizeof(Bv) + sizeof(bv_data_unit)*bits_in_units(bv2->len());
    byte_ptr site = byte_alloc_traits::allocate(ba, needed);
    new (site.get()) Bv{*bv2};
    return byte_ptr_traits::rebind<Bv>{byte_ptr_traits::rebind<void>{site}};
}

//=================================================================================================
// Bit vector type
//
Bv::Bv (size_t len)
{
    length = len;
    for (size_t i=0; i<bits_in_units(length); i++)
        data[i] = 0;  // init to zeros
    validate();
}

Bv::Bv (const string& bs) : Bv{bs.length()}
{
    setter(bs);
    validate();
}

Bv::Bv (size_t len, unsigned i) : Bv{len}
{
    setter(i);
    validate();
}

Bv::Bv (const Bv& bv2)
{
    length = bv2.length;
    for (size_t i=0; i<bits_in_units(length); i++)
        data[i] = bv2.data[i];
    validate();
}

Bv& Bv::operator=(const Bv& bv2)
{
    length = bv2.length;
    for (size_t i=0; i<bits_in_units(length); i++)
        data[i] = bv2.data[i];
    validate();
    return *this;
}

size_t Bv::len () const
{
    return length;
}

bool Bv::getter (size_t ind) const
{
    /*
     * index x is stored in get_index_prefix(x) byte.
     * index x is stored at get_index_suffix(x) bit.
     *
     * if index x is 1 then the extraction is equal to the mask;
     * otherwise it is zero.
     */
    assert (ind < length);
    return (data[get_index_prefix(ind)] &
            get_index_mask(get_index_suffix(ind))) != 0;
}

void Bv::setter (size_t ind, bool val)
{
    /*
     * set x to val.
     * if val is true then override with or-ing a mask of 1.
     * if val is false then override with and-ing a mask of 0.
     */
    assert (ind < length);
    if (val)
        data[get_index_prefix(ind)] |=
            get_index_mask(get_index_suffix(ind));
    else
        data[get_index_prefix(ind)] &=
            ~get_index_mask(get_index_suffix(ind));
}

void Bv::flipper (size_t ind)
{
    /*
     * flip value at ind.
     * x xor 1 is ~x while x xor 0 is x
     */
    data[get_index_prefix(ind)] ^=
        get_index_mask(get_index_suffix(ind));
}

namespace
{
    inline void validate_bitstring (size_t bv_len, const string& bs)
    {
        assert (bv_len == bs.length());
        for ([[maybe_unused]] const auto& b : bs)
            assert(b == '0' || b == '1');
    }
}
void Bv::setter (const string& bs)
{
    /*
     * set by bit string.
     */
    validate_bitstring(len(), bs);

    auto bit=begin(); auto sit=bs.begin();
    for (;bit!=end(); ++bit, ++sit)
        bit.setbit(*sit == '1');
}

namespace
{
    inline void validate_unsigned_setter (size_t bv_len, unsigned i)
    {
        size_t size = 0;
        for (; i>0; size++, i>>=1);
        assert(bv_len >= size);
    }
}
void Bv::setter (unsigned i)
{
    /*
     * set by unsigned int.
     */
    validate_unsigned_setter(len(), i);

    for (auto rbit=(--end()); rbit!=(--begin()); --rbit, i>>=1)
        rbit.setbit(1&i);
}

string Bv::to_string () const
{
    string s = ""s;
    for (bool b : *this)
        s += (b ? '1' : '0');
    return s;
}

string Bv::to_string_pretty () const
{
    constexpr size_t segment = 4;
    string s = ""s;
    for (size_t i=1; auto b : *this)
    {
        s += (b ? '1' : '0');
        if (i%segment == 0)
            s += " ";
        i++;
    }
    s += '\n';
    return s;
}

bool Bv::operator [] (size_t ind) const
{
    return getter(ind);
}

bool operator == (const Bv_ptr bv1, const Bv_ptr bv2)
{
    assert(bv1->length == bv2->length);
    auto it1=bv1->data, it2=bv2->data;
    for (size_t i=0; i<bits_in_units(bv1->length); i++, it1++, it2++)
        if (*it1 != *it2)
            return false;
    return true;
}

bool operator != (const Bv_ptr bv1, const Bv_ptr bv2)
{
    return !(bv1 == bv2);
}

bool operator < (const Bv_ptr bv1, const Bv_ptr bv2)
{
    assert(bv1->length == bv2->length);
    auto it1=bv1->data, it2=bv2->data;
    for (size_t i=0; i<bits_in_units(bv1->length); i++, it1++, it2++)
        if ((*it1 & *it2) != *it1)
            return false;
    return true;
}

bool operator <= (const Bv_ptr bv1, const Bv_ptr bv2)
{
    return (bv1 == bv2) || (bv1 < bv2);
}

Bv_ptr operator ~ (const Bv_ptr bv)
{
    Bv_ptr tmp = mkBv(bv);
    auto it=tmp->data;
    for (size_t i=0; i<bits_in_units(tmp->length); i++, it++)
        *it = ~*it;
    // reset trailing garbage bits to zeros; this is the only place where it could be set to ones
    it--; *it = reset_data_trail(*it, tmp->length);
    return tmp;
}

Bv_ptr operator & (const Bv_ptr bv1, const Bv_ptr bv2)
{
    assert(bv1->length == bv2->length);
    Bv_ptr tmp = mkBv(bv1->length);
    auto it = tmp->data; auto it1=bv1->data, it2=bv2->data;
    for (size_t i=0; i<bits_in_units(tmp->length); i++, it++, it1++, it2++)
        *it = *it1 & *it2;
    return tmp;
}

Bv_ptr operator | (const Bv_ptr bv1, const Bv_ptr bv2)
{
    assert(bv1->length == bv2->length);
    Bv_ptr tmp = mkBv(bv1->length);
    auto it = tmp->data; auto it1=bv1->data, it2=bv2->data;
    for (size_t i=0; i<bits_in_units(tmp->length); i++, it++, it1++, it2++)
        *it = *it1 | *it2;
    return tmp;
}

Bv_ptr operator ^ (const Bv_ptr bv1, const Bv_ptr bv2)
{
    assert(bv1->length == bv2->length);
    Bv_ptr tmp = mkBv(bv1->length);
    auto it = tmp->data; auto it1=bv1->data, it2=bv2->data;
    for (size_t i=0; i<bits_in_units(tmp->length); i++, it++, it1++, it2++)
        *it = *it1 ^ *it2;
    return tmp;
}

//=================================================================================================
// Iterate over bits
//
Bv::BitIterator Bv::begin() const { return BitIterator((bv_data_unit*)(data), 0); } // relies on UB
Bv::BitIterator Bv::end()   const { return BitIterator((bv_data_unit*)(data), length); } // relies on UB

Bv::BitIterator::BitIterator (bv_data_unit* ptr, size_t ind) : p(ptr), i(ind)
{
    for (size_t n=0; n<get_index_prefix(i); n++, p++);
    mask_ind = get_index_mask(get_index_suffix(i));
}

Bv::BitIterator& Bv::BitIterator::operator++() // prefix increment
{
    if (mask_ind & 1) // end bit reached, go to next byte
    {
        mask_ind = one_zeros_mask;
        p++;
    }
    else // move mask right by 1
    {
        mask_ind >>= 1;
    }
    i++;
    return *this;
}

Bv::BitIterator Bv::BitIterator::operator++(int) // postfix increment
{
    BitIterator tmp = *this;
    ++(*this);
    return tmp;
}

Bv::BitIterator& Bv::BitIterator::operator--() // prefix decrement
{
    if (mask_ind == one_zeros_mask) // first bit reached, go to previous byte
    {
        mask_ind = 1;
        p--;
    }
    else // move mask left by 1
    {
        mask_ind <<= 1;
    }
    i--;
    return *this;
}

Bv::BitIterator Bv::BitIterator::operator--(int) // postfix decrement
{
    BitIterator tmp = *this;
    --(*this);
    return tmp;
}

// iterator is at the same location if the data pointer and the index are the same
bool operator == (const Bv::BitIterator& bit1, const Bv::BitIterator& bit2)
{
    return bit1.p == bit2.p && bit1.i == bit2.i;
}

bool operator != (const Bv::BitIterator& bit1, const Bv::BitIterator& bit2)
{
    return bit1.p != bit2.p || bit1.i != bit2.i;
}

// returned is read-only, do not assign to it
const bool Bv::BitIterator::operator*() const
{
    return (*p & mask_ind) != 0;
}

// set bit at current index to val
void Bv::BitIterator::setbit (bool val)
{
    val ? *p |= mask_ind : *p &= ~mask_ind;
}
