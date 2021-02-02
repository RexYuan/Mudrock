
#include "bv.hxx"

//=================================================================================================
// Calculate bit-byte positions
//
// local only functions. not to be used outside this file
namespace
{
    // how many bytes are needed to store `bits` bits.
    //
    // n bits need (n-1)/8+1 bytes to store.
    // (n-1)/8+1 = (n/8 - 1/8 + 8/8) = (n/8 + 7/8) = (n+7)/8
    // x >> 3 is x/8.
    inline size_t bits_in_bytes (size_t bits)
    {
        /*
        * 1~8 minus 1
        * 1 | 0000
        * 2 | 0001
        * 3 | 0010
        * 4 | 0011
        * 5 | 0100
        * 6 | 0101
        * 7 | 0110
        * 8 | 0111
        *     0
        * 1~8 minus 1 right shift 3 is 0
        * and requires 1 byte to store.
        * 
        * 9~16 minus 1
        * 9 | 1000
        * 10| 1001
        * 11| 1010
        * 12| 1011
        * 13| 1100
        * 14| 1101
        * 15| 1110
        * 16| 1111
        *     1
        * 9~16 minus 1 right shift 3 is 1
        * and requires 2 byte to store.
        * 
        * 17~24 minus 1
        * 17| 1 0000
        * 18| 1 0001
        * 19| 1 0010
        * 20| 1 0011
        * 21| 1 0100
        * 22| 1 0101
        * 23| 1 0110
        * 24| 1 0111
        *     1 0
        * 17~24 minus 1 right shift 3 is 2
        * and requires 3 byte to store.
        */
        assert (bits > 0);
        assert ((bits+7)>>3 > 0);
        return (bits+7) >> 3;
    }

    // which byte is `index`th bit stored
    //
    // x is stored at byte x/8.
    // x >> 3 is x/8.
    inline size_t get_index_prefix (size_t index)
    {
        /*
        * 0~ 7 is stored at byte 0
        * 8~15 is stored at byte 1
        */
        return index >> 3;
    }

    // which bit is `index`th bit stored in its byte
    //
    // x is stored at index x%8 of byte x/8
    // x & 7 is x%8.
    inline size_t get_index_suffix (size_t index)
    {
        /*
        * 0~ 7 is stored at index 0~7 of byte 0
        * 8~15 is stored at index 0~7 of byte 1
        */
        return index & 7;
    }

    // mask for extracting `index`th bit from its byte
    //
    // the index counts from left to right.
    // generate a bitmask for extracting indexed bit.
    inline size_t get_index_mask (size_t index)
    {
        assert (0 <= index && index <= 7);
        return 0b10000000 >> index;
    }
}

//=================================================================================================
// Bit vector type
//
Bv::Bv (size_t len)
{
    length = len;
    data = new unsigned char [bits_in_bytes(length)]{}; // init to zeros
    assert (data);
}

Bv::Bv (std::string bs)
{
    for (const auto& b : bs) assert (b == '0' || b == '1');
    length = bs.length();
    data = new unsigned char [bits_in_bytes(length)]{};
    assert (data);
    setter(bs);
}

Bv::~Bv ()
{
    delete [] data;
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

void Bv::setter (std::string bs)
{
    /*
     * set by bit string.
     */
    assert (bs.length() == length);
    auto bit=begin(); auto sit=bs.begin();
    for (;bit!=end(); ++bit, ++sit)
        bit.setbit(*sit == '1');
}

std::string Bv::to_string () const
{
    std::string s = "";
    for (bool b : *this)
        s += (b ? "1" : "0");
    return s;
}

//=================================================================================================
// Iterate over bits
//
Bv::BitIterator::BitIterator (unsigned char* ptr, size_t ind) : p(ptr), i(ind)
{
    for (size_t n=0; n<get_index_prefix(i); n++, p++);
    mask_ind = get_index_mask(get_index_suffix(i));
}

Bv::BitIterator& Bv::BitIterator::operator++() // prefix increment
{
    if (mask_ind & 1) // end bit reached, go to next byte
    {
        mask_ind = 0b10000000;
        p++;
    }
    else // move mask right by 1
    {
        mask_ind >>= 1;
    }
    i++;
    return *this;
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
