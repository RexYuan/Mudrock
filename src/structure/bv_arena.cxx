
#include "bv_arena.hxx"

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

    // how many units are needed to store `bits` bits
    inline constexpr const size_t bits_in_units (size_t bits)
    {
        assert (bits > 0U);
        return (bits+data_unit_bits-1U) / data_unit_bits;
    }
}

Bv* BvArena::mkBv (size_t len, unsigned i)
{
    MemArena::mem_unit* site = ma.alloc(sizeof(Bv) +
        sizeof(Bv::data_unit)*bits_in_units(len));
    void* data_site = (Bv*)site + 1;
    return new (site) Bv{data_site, len, i};
}
