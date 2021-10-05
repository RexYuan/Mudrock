
#pragma once

#include <cassert>
#include <cstddef>
// ptrdiff_t

#include <iterator>
using std::random_access_iterator_tag;
using std::distance;
#include <memory>
using std::addressof;
#include <type_traits>
using std::enable_if;
using std::is_same;
using std::remove_cv;
using std::is_void;
using std::is_convertible;
using std::common_type;

#include "arena.hxx"

// singleton arena pointer
template <typename T>
class ArenaPtrBase
{
    template <typename U> friend struct ArenaPtrBase;

protected:
    static const ptrdiff_t null_value = -1;
    ptrdiff_t offset; // offset in bytes from the beginning of host arena

    bool offset_is_ok () // within range of host arena
    {
        return (offset == null_value)
            || (0 <= offset && offset < static_cast<ptrdiff_t>(SingletonArena::Get().get_size()));
    }

public:
    bool null () const noexcept
    {
        return offset == null_value;
    }

    T* get () const noexcept // return native pointer
    {
        return null() ? nullptr : reinterpret_cast<T*>(SingletonArena::Get().get_data()+offset);
    }

    // Constructors
    ArenaPtrBase () noexcept
    : offset{null_value}
    { }

    ArenaPtrBase (T* native) noexcept
    : offset{native == nullptr ? null_value : reinterpret_cast<unsigned char*>(native)-SingletonArena::Get().get_data()}
    { assert(offset_is_ok()); }

    ArenaPtrBase (const ArenaPtrBase& rhs) noexcept
    : offset{rhs.null() ? null_value : rhs.offset}
    { assert(offset_is_ok()); }

    // conversion from non cv to cv
    template <typename U,
              typename enable_if<!is_same<T, U>::value && is_same<typename remove_cv<T>::type, U>::value, int>::type = 0>
    ArenaPtrBase (const ArenaPtrBase<U>& rhs) noexcept
    : offset{rhs.null() ? null_value : rhs.offset}
    { assert(offset_is_ok()); }

    // converison from convertible non void
    template <typename U,
              typename enable_if<!is_same<typename remove_cv<T>::type, typename remove_cv<U>::type>::value &&
                                 !is_void<U>::value && is_convertible<U*, T*>::value, int>::type = 0>
    ArenaPtrBase (const ArenaPtrBase<U>& rhs) noexcept
    : offset{rhs.null() ? null_value : rhs.offset}
    { assert(offset_is_ok()); }

    // NullablePointer requirements
    explicit operator bool() const noexcept
    {
        return !null();
    }

    ArenaPtrBase& operator=(const ArenaPtrBase& rhs) noexcept
    {
        if (this != &rhs)
            offset = rhs.null() ? null_value : rhs.offset;
        return *this;
    }

    ArenaPtrBase& operator=(std::nullptr_t) noexcept
    {
        offset = null_value;
        return *this;
    }
};

template <typename T> struct ArenaPtr;

template <>
struct ArenaPtr<void> : ArenaPtrBase<void>
{
    ArenaPtr () = default;
    using ArenaPtrBase<void>::ArenaPtrBase;
    using ArenaPtrBase<void>::operator=;
};

template <>
struct ArenaPtr<const void> : ArenaPtrBase<const void>
{
    ArenaPtr () = default;
    using ArenaPtrBase<void const>::ArenaPtrBase;
    using ArenaPtrBase<void const>::operator=;
};


template <typename T>
struct ArenaPtr : ArenaPtrBase<T>
{
    ArenaPtr () = default;
    using ArenaPtrBase<T>::ArenaPtrBase;
    using ArenaPtrBase<T>::operator=;

    // conversion from void
    explicit ArenaPtr (const ArenaPtr<void>& rhs) noexcept
    : ArenaPtrBase<T>(static_cast<T*>(rhs.get()))
    { }

    explicit ArenaPtr (const ArenaPtr<const void>& rhs) noexcept
    : ArenaPtrBase<T>(static_cast<const T*>(rhs.get()))
    { }

    // conversion to void
    operator ArenaPtr<void>() const noexcept
    {
        return {this->get()};
    }

    operator ArenaPtr<void const>() const noexcept
    {
        return {this->get()};
    }

    // For pointer traits
    // Constructs a dereferenceable pointer or pointer-like object ("fancy pointer") to its argument.
    static ArenaPtr pointer_to (T& x) { return ArenaPtr(addressof(x)); }
    static T* to_address (ArenaPtr p) noexcept { return p.get(); } // unimplemented

    // Random access iterator requirements (members)
    using iterator_category = random_access_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = ArenaPtr<T>;

    ArenaPtr operator+(ptrdiff_t n) const
    {
        return ArenaPtr{this->get() + n};
    }

    ArenaPtr& operator+=(ptrdiff_t n)
    {
        this->offset += sizeof(T)*n; // offset is byte address
        assert(ArenaPtrBase<T>::offset_is_ok());
        return *this;
    }

    ArenaPtr operator-(ptrdiff_t n) const
    {
        return ArenaPtr{this->get() - n};
    }

    ArenaPtr& operator-=(std::ptrdiff_t n)
    {
        this->offset -= sizeof(T)*n; // offset is byte address
        assert(ArenaPtrBase<T>::offset_is_ok());
        return *this;
    }

    ptrdiff_t operator-(ArenaPtr const & rhs) const
    {
        return distance(rhs.get(), this->get());
    }

    ArenaPtr& operator++()
    {
        this->offset += sizeof(T);
        return *this;
    }

    ArenaPtr & operator--()
    {
        this->offset -= sizeof(T);
        return *this;
    }

    ArenaPtr operator++(int)
    {
        ArenaPtr tmp(*this);
        ++*this;
        return tmp;
    }

    ArenaPtr operator--(int)
    {
        ArenaPtr tmp(*this);
        --*this;
        return tmp;
    }

    T* operator->() const noexcept { return this->get(); }
    T& operator*() const noexcept { return *this->get(); }
    T& operator[](std::size_t i) const noexcept { return this->get()[i]; }
};

// Random access iterator requirements (non-members)
#define DEFINE_OPERATOR(oper, op, expr)                                                              \
  template <typename T>                                                                              \
  bool oper (ArenaPtr<T> const & lhs, ArenaPtr<T> const & rhs) noexcept                                  \
  { return expr; }                                                                                   \
  template <typename T>                                                                              \
  bool oper (ArenaPtr<T> const & lhs, typename common_type<ArenaPtr<T>>::type const & rhs) noexcept \
  { return lhs op rhs; }                                                                             \
  template <typename T>                                                                              \
  bool oper (typename common_type<ArenaPtr<T>>::type const & lhs, ArenaPtr<T> const & rhs) noexcept \
  { return lhs op rhs; }
DEFINE_OPERATOR(operator==, ==, (lhs.get() == rhs.get()))
DEFINE_OPERATOR(operator!=, !=, (lhs.get() != rhs.get()))
DEFINE_OPERATOR(operator<,  <,  (lhs.get() <  rhs.get()))
DEFINE_OPERATOR(operator<=, <=, (lhs.get() <= rhs.get()))
DEFINE_OPERATOR(operator>,  >,  (lhs.get() >  rhs.get()))
DEFINE_OPERATOR(operator>=, >=, (lhs.get() >= rhs.get()))
#undef DEFINE_OPERATOR


