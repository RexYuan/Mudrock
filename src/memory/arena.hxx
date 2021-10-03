
#pragma once

#include <cassert>
#include <cstdlib>
using std::realloc;
using std::free;
#include <new>
using std::bad_alloc;

#include <string>
using std::string;
using namespace std::string_literals;

#include <iostream>
using std::ostream;
using std::cout;
using std::endl;

template <typename T> struct ArenaPtrBase;

// byte arena
class Arena
{
    template <typename T> friend struct ArenaPtrBase;

    unsigned char* data;
    std::size_t size; // capacity in bytes
    std::size_t offset; // start of free region
    std::size_t freed_size; // num of freed bytes

    unsigned char* get_data () const
    {
        return data;
    }

    std::size_t get_size () const
    {
        return size;
    }

public:
    explicit Arena (std::size_t s)
    : data(nullptr)
    , size(0)
    , offset(0)
    , freed_size(0)
    {
        capacity(s);
        // cout << "arena[" << this << "] of size " << size << " created.\n";
    }

    Arena (const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    ~Arena()
    {
        // cout << "arena[" << this << "] destroyed; final fill level was: " << offset << "; freed " << freed_size << "\n";
        free(data);
    }

    void capacity (size_t min_cap)
    {
        size_t cap = size;
        if (cap >= min_cap)
            return;

        cout << "reallocing " << cap;

        size_t prev_cap;
        while (cap < min_cap)
        {
            prev_cap = cap;
            ((cap <<= 1) += 2) &= ~1; // grow by approx doubling

            if (cap <= prev_cap)
                throw bad_alloc();
        }

        if (cap <= 0)
            throw bad_alloc();
        cout << " => " << cap << endl;

        unsigned char* retptr = (unsigned char*)realloc(data, sizeof(unsigned char) * cap);
        if (retptr == nullptr)
            throw bad_alloc();

        cout << "realloc success" << endl;
        data = retptr;
        size = cap;
    }

    void* allocate (size_t n, size_t a) // allocate `n` bytes with `a` alignment
    {
        offset = (offset + a - 1) / a * a; // align offset

        // cout << "arena[" << this << "] allocating " << n << " bytes at offset " << offset << ".\n";

        if (offset + n > size)
            capacity(offset+n+1);

        void * result = data + offset;
        offset += n;
        return result;
    }

    void deallocate(void*, size_t n)
    {
        // std::cout << "arena[" << this << "] may deallocate " << n << " bytes.\n";
        freed_size++;
    }
};

struct SingletonArena
{
    static Arena& Get ()
    {
        static Arena ar{2};
        return ar;
    }

    SingletonArena () {}
    SingletonArena           (const SingletonArena&) = delete;
    SingletonArena& operator=(const SingletonArena&) = delete;
    SingletonArena           (SingletonArena&&) = delete;
    SingletonArena& operator=(SingletonArena&&) = delete;
};
