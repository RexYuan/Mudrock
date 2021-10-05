
#include "bf.hxx"

//=================================================================================================
// Pointer manipulation helpers
//
Bf_ptr v (bool b)
{
    static Bf_alloc ba; // TODO: optimize by cache
    Bf_ptr site = Bf_alloc_traits::allocate(ba, 1);
    new (site.get()) Bf{b};
    return site;
}

Bf_ptr v (int i)
{
    static Bf_alloc ba; // TODO: optimize by cache
    Bf_ptr site = Bf_alloc_traits::allocate(ba, 1);
    new (site.get()) Bf{i};
    return site;
}

Bf_ptr neg (Bf_ptr bf)
{
    switch (bf->t)
    {
    case Conn::Top: return v(false);
    case Conn::Bot: return v(true);
    case Conn::Not: return bf->get_sub();
    default:
        static Bf_alloc ba;
        Bf_ptr site = Bf_alloc_traits::allocate(ba, 1);
        new (site.get()) Bf{Conn::Not, bf};
        return site;
    }
}

Bf_ptr conj (Bf_ptr bf1, Bf_ptr bf2)
{
    if      (bf1->t == Conn::Bot ||
             bf2->t == Conn::Bot) return v(false);
    else if (bf1->t == Conn::Top) return bf2;
    else if (bf2->t == Conn::Top) return bf1;
    else
    {
        static Bf_alloc ba;
        Bf_ptr tmp = Bf_alloc_traits::allocate(ba, 1);
        new (tmp.get()) Bf{Conn::And};
        if (bf1->t == Conn::And)
            for (auto s : bf1->get_subs())
                tmp->push_sub(s);
        else
            tmp->push_sub(bf1);
        if (bf2->t == Conn::And)
            for (auto s : bf2->get_subs())
                tmp->push_sub(s);
        else
            tmp->push_sub(bf2);
        return tmp;
    }
}

Bf_ptr disj (Bf_ptr bf1, Bf_ptr bf2)
{
    if      (bf1->t == Conn::Top ||
             bf2->t == Conn::Top) return v(true);
    else if (bf1->t == Conn::Bot) return bf2;
    else if (bf2->t == Conn::Bot) return bf1;
    else
    {
        static Bf_alloc ba;
        Bf_ptr tmp = Bf_alloc_traits::allocate(ba, 1);
        new (tmp.get()) Bf{Conn::Or};
        if (bf1->t == Conn::Or)
            for (auto s : bf1->get_subs())
                tmp->push_sub(s);
        else
            tmp->push_sub(bf1);
        if (bf2->t == Conn::Or)
            for (auto s : bf2->get_subs())
                tmp->push_sub(s);
        else
            tmp->push_sub(bf2);
        return tmp;
    }
}

//=================================================================================================
// Boolean formula type
//
const Conn Bf::get_type () const
{
    return t;
}

const bool Bf::get_bool () const
{
    assert(t == Conn::Top || t == Conn::Bot);
    switch (t)
    {
        case Conn::Top: return true;
        case Conn::Bot: return false;
        default:        break;
    }
    throw InvalidBfConn("Must be bool constants."s);
}

const int Bf::get_int () const
{
    assert(t == Conn::Base);
    return get<int>(sub);
}

const Bf_ptr& Bf::get_sub () const
{
    assert(t == Conn::Not);
    return get<Bf_ptr>(sub);
}

const Bf_ptr_vector& Bf::get_subs () const
{
    assert(t == Conn::And || t == Conn::Or);
    return get<Bf_ptr_vector>(sub);
}

void Bf::push_sub (Bf_ptr bf)
{
    assert(holds_alternative<Bf_ptr_vector>(sub));
    get<Bf_ptr_vector>(sub).push_back(bf);
}

string Bf::to_string ()
{
    switch (t)
    {
    case Conn::Top:  return "T"s;
    case Conn::Bot:  return "F"s;
    case Conn::Base: return std::to_string(get_int());
    case Conn::Not:  return "~"s + get_sub()->to_string();
    case Conn::And:
    {
        string tmp = "("s;
        for (Bf_ptr s : get_subs())
            tmp.append(s->to_string() + "&"s);
        tmp.pop_back();
        tmp.append(")"s);
        return tmp;
    }
    case Conn::Or:
    {
        string tmp = "("s;
        for (Bf_ptr s : get_subs())
            tmp.append(s->to_string() + "|"s);
        tmp.pop_back();
        tmp.append(")"s);
        return tmp;
    }
    }
    throw InvalidBfConn("Unmatched cases."s);
}
