
#pragma once

#include <type_traits>
using std::true_type;

#include "arena.hxx"
#include "arena_ptr.hxx"

// singleton arena allocator
template <typename T>
struct ArenaAlloc
{
    template <typename U> friend struct ArenaAlloc;

    using value_type = T;
    using pointer = ArenaPtr<T>;

    using propagate_on_container_copy_assignment = true_type;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap = true_type;

    explicit ArenaAlloc () {}

    template <typename U>
    ArenaAlloc (ArenaAlloc<U> const & rhs) {}

    pointer allocate(size_t n)
    {
        return pointer(static_cast<value_type*>(arena.allocate(n * sizeof(T), alignof(T))));
    }

    void deallocate(pointer p, size_t n)
    {
        arena.deallocate(reinterpret_cast<void*>(p.get()), n * sizeof(T));
    }

    using is_always_equal = true_type;

    template <typename U>
    bool operator==(ArenaAlloc<U> const & rhs) const
    {
        return true;
    }

    template <typename U>
    bool operator!=(ArenaAlloc<U> const & rhs) const
    {
        return false;
    }

private:
    Arena& arena = SingletonArena::Get();
};
