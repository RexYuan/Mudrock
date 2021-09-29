
#include "old_arena.hxx"

MemArena::MemArena (size_t start_cap) :
arena{nullptr}, sz{0}, cap{0}
{
    capacity(start_cap);
}

MemArena::~MemArena ()
{
    free(arena);
}

void MemArena::capacity (size_t min_cap)
{
    if (cap >= min_cap)
        return;

    size_t prev_cap;
    while (cap < min_cap)
    {
        prev_cap = cap;
        ((cap <<= 1) += 2) &= ~1; // grow by approx doubling

        if (cap <= prev_cap)
            throw MemoryOverflow("Cap overflow"s);
    }

    assert(cap > 0);
    std::cout << "reallocing " << prev_cap << " => " << cap << std::endl;
    arena = (mem_unit*)realloc(arena, sizeof(mem_unit) * cap);
    std::cout << "realloced " << std::endl;
    if (arena == nullptr)
        throw MemoryOverflow("Arena overflow"s);
}

MemArena::mem_unit* MemArena::alloc (size_t bytes)
{
    size_t prev_sz = sz;
    sz += (bytes+sizeof(mem_unit)-1) / sizeof(mem_unit);
    if (sz < prev_sz)
        throw MemoryOverflow("Sz overflow"s);

    capacity(sz);

    return arena + prev_sz;
}
