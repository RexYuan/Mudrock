
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
    inline constinit const size_t data_unit_bits = bits_in_bytes(sizeof(Bv::data_unit));

    // mask of 1000...
    inline constinit const Bv::data_unit one_zeros_mask = 1 << (data_unit_bits-1);

    // mask of 1111...
    inline constinit const Bv::data_unit ones_mask = ~0;

    // how many units are needed to store `bits` bits
    inline constexpr const size_t bits_in_units (size_t bits)
    {
        assert (bits > 0);
        return (bits+data_unit_bits-1) / data_unit_bits;
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
        assert (0 <= index && index <= data_unit_bits);
        return one_zeros_mask >> index;
    }

    // mask for removing the rightmost n bits
    inline constexpr const size_t get_right_zeros_mask (size_t len)
    {
        assert (0 <= len && len <= data_unit_bits);
        return ones_mask << len;
    }

    // reset data trail to zeros
    inline constexpr Bv::data_unit reset_data_trail (Bv::data_unit u, size_t total_bit_len)
    {
        return u & get_right_zeros_mask(data_unit_bits-total_bit_len%data_unit_bits);
    }
}

//=================================================================================================
// Bit vector type
//
Bv::Bv (size_t len)
{
    length = len;
    data = new data_unit [bits_in_units(length)]{}; // init to zeros
    assert (data);
}

Bv::Bv (string bs)
{
    for (const auto& b : bs) assert (b == '0' || b == '1');
    length = bs.length();
    data = new data_unit [bits_in_units(length)]{};
    assert (data);
    setter(bs);
}

Bv::~Bv ()
{
    delete [] data;
}

Bv::Bv (const Bv& bv2)
{
    length = bv2.length;
    data = new data_unit [bits_in_units(length)]{};
    assert (data);
    for (size_t i=0; i<bits_in_units(length); i++)
        data[i] = bv2.data[i];
}

Bv& Bv::operator=(const Bv& bv2)
{
    delete [] data;
    length = bv2.length;
    data = new data_unit [bits_in_units(length)]{};
    assert (data);
    for (size_t i=0; i<bits_in_units(length); i++)
        data[i] = bv2.data[i];
    return *this;
}

Bv::Bv (Bv&& bv2)
{
    length = bv2.length;
    data = bv2.data;
    bv2.length = 0;
    bv2.data = nullptr;
}

Bv& Bv::operator=(Bv&& bv2)
{
    length = bv2.length;
    data = bv2.data;
    assert (data);
    bv2.length = 0;
    bv2.data = nullptr;
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

void Bv::setter (string bs)
{
    /*
     * set by bit string.
     */
    assert (bs.length() == length);
    auto bit=begin(); auto sit=bs.begin();
    for (;bit!=end(); ++bit, ++sit)
        bit.setbit(*sit == '1');
}

Bv::operator bool () const
{
    return (len() > 0) && (data != nullptr);
}

string Bv::to_string () const
{
    string s = "";
    for (bool b : *this)
        s += (b ? "1" : "0");
    return s;
}

string Bv::to_string_pretty () const
{
    constexpr size_t segment = 4;
    string s = "";
    for (size_t i=1; auto b : *this)
    {
        s += (b ? "1" : "0");
        if (i%segment == 0)
            s += " ";
        i++;
    }
    s += "\n";
    return s;
}

bool Bv::operator [] (size_t ind) const
{
    return getter(ind);
}

bool operator == (const Bv& bv1, const Bv& bv2)
{
    assert(bv1.length == bv2.length);
    auto it1=bv1.data, it2=bv2.data;
    for (size_t i=0; i<bits_in_units(bv1.length); i++, it1++, it2++)
        if (*it1 != *it2)
            return false;
    return true;
}

bool operator != (const Bv& bv1, const Bv& bv2)
{
    return !(bv1 == bv2);
}

bool operator <  (const Bv& bv1, const Bv& bv2)
{
    assert(bv1.length == bv2.length);
    auto it1=bv1.data, it2=bv2.data;
    for (size_t i=0; i<bits_in_units(bv1.length); i++, it1++, it2++)
        if ((*it1 & *it2) != *it1)
            return false;
    return true;
} 

bool operator <= (const Bv& bv1, const Bv& bv2)
{
    return (bv1 == bv2) || (bv1 < bv2);
}

Bv& Bv::operator &= (const Bv& bv2)
{
    assert(length == bv2.length);
    auto it=data, it2=bv2.data;
    for (size_t i=0; i<bits_in_units(length); i++, it++, it2++)
        *it &= *it2;
    return *this;
}

Bv& Bv::operator |= (const Bv& bv2)
{
    assert(length == bv2.length);
    auto it=data, it2=bv2.data;
    for (size_t i=0; i<bits_in_units(length); i++, it++, it2++)
        *it |= *it2;
    return *this;
}

Bv& Bv::operator ^= (const Bv& bv2)
{
    assert(length == bv2.length);
    auto it=data, it2=bv2.data;
    for (size_t i=0; i<bits_in_units(length); i++, it++, it2++)
        *it ^= *it2;
    return *this;
}

Bv operator ~ (Bv bv)
{
    auto it=bv.data;
    for (size_t i=0; i<bits_in_units(bv.length); i++, it++)
        *it = ~*it;
    // reset trailing garbage bits to zeros; this is the only place where it could be set to ones
    it--; *it = reset_data_trail(*it, bv.length);
    return bv;
}

Bv operator & (Bv bv1, Bv bv2)
{
    return bv1 &= bv2;
}

Bv operator | (Bv bv1, Bv bv2)
{
    return bv1 |= bv2;
}

Bv operator ^ (Bv bv1, Bv bv2)
{
    return bv1 ^= bv2;
}

//=================================================================================================
// Iterate over bits
//
Bv::BitIterator::BitIterator (data_unit* ptr, size_t ind) : p(ptr), i(ind)
{
    for (size_t n=0; n<get_index_prefix(i); n++, p++);
    mask_ind = get_index_mask(get_index_suffix(i));
}

Bv::BitIterator Bv::begin() const
{
    return BitIterator(data, 0);
}

Bv::BitIterator Bv::end() const
{
    return BitIterator(data, length);
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
bool operator == (const Bv::BitIterator& a, const Bv::BitIterator& b)
{
    return a.p == b.p && a.i == b.i;
}

bool operator != (const Bv::BitIterator& a, const Bv::BitIterator& b)
{
    return a.p != b.p || a.i != b.i;
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
