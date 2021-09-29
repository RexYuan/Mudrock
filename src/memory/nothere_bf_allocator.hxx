
#pragma once

//#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

class Arena
{
    unsigned char * data;
    std::size_t size;
    std::size_t offset;

public:
    explicit Arena(std::size_t s)
    //: data(static_cast<unsigned char *>(::operator new(s)))
    : data(nullptr)
    , size(0)
    , offset(0)
    {
        capacity(s);
        std::cout << "arena[" << this << "] of size " << size << " created.\n";
        //std::cout << (void*)data << std::endl;
    }

    Arena(Arena const &) = delete;
    Arena & operator=(Arena const &) = delete;

    ~Arena()
    {
        std::cout << "arena[" << this << "] destroyed; final fill level was: " << offset << "\n";
        //::operator delete(data);
        std::free(data);
    }

    void capacity (size_t min_cap)
    {
        std::size_t cap = size;
        if (cap >= min_cap)
            return;

        size_t prev_cap;
        while (cap < min_cap)
        {
            prev_cap = cap;
            ((cap <<= 1) += 2) &= ~1; // grow by approx doubling

            if (cap <= prev_cap)
                throw std::bad_alloc();
        }

        assert(cap > 0);
        std::cout << "reallocing " << prev_cap << " => " << cap << std::endl;

        data = (unsigned char*)std::realloc(data, sizeof(unsigned char) * cap);

        //::operator delete(data);
        //data = static_cast<unsigned char *>(::operator new(cap));

        std::cout << "realloced " << std::endl;
        if (!data)
            throw std::bad_alloc();
        size = cap;
    }

    void * allocate(std::size_t n, std::size_t a) // a is alignment
    {
        offset = (offset + a - 1) / a * a;

        std::cout << "arena[" << this << "] allocating " << n << " bytes at offset " << offset << ".\n";

        while (offset + n > size)
        {
            capacity(offset+n+1);
            //throw std::bad_alloc();
        }

        void * result = data + offset;
        offset += n;

        std::cout << "arena[" << this << "] allocated " << n << " bytes at offset " << offset << ".\n";

        return result;
    }

    void deallocate(void *, std::size_t n)
    {
        std::cout << "arena[" << this << "] may deallocate " << n << " bytes.\n";
    }
};

#include <type_traits>

template <typename T>
struct ArenaAllocator
{
    template <typename U> friend struct ArenaAllocator;

    using value_type = T;
    using pointer = T *;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    explicit ArenaAllocator(Arena * a) : arena(a) {}

    template <typename U>
    ArenaAllocator(ArenaAllocator<U> const & rhs) : arena(rhs.arena) {}

    pointer allocate(std::size_t n)
    {
        return static_cast<pointer>(arena->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(pointer p, std::size_t n)
    {
        arena->deallocate(p, n * sizeof(T));
    }

    template <typename U>
    bool operator==(ArenaAllocator<U> const & rhs) const
    {
        return arena == rhs.arena;
    }

    template <typename U>
    bool operator!=(ArenaAllocator<U> const & rhs) const
    {
        return arena != rhs.arena;
    }

private:
    Arena * arena;
};

#include <scoped_allocator>

template <typename T>
using SA = std::scoped_allocator_adaptor<ArenaAllocator<T>>;

struct SingletonSA
{
    static SA<void>& Get ()
    {
        static Arena ar{2};
        static SA<void> sa{&ar};
        return sa;
    }

    SingletonSA () {}
    SingletonSA           (const SingletonSA&) = delete;
    SingletonSA& operator=(const SingletonSA&) = delete;
    SingletonSA           (SingletonSA&&) = delete;
    SingletonSA& operator=(SingletonSA&&) = delete;
};
