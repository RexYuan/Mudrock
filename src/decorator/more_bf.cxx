
#include "more_bf.hxx"

bool contains (const Bf_ptr& bf, int v)
{
    switch (bf->t)
    {
    case Conn::Base: return (bf->get_int() == v);
    case Conn::Top:  [[fallthrough]];
    case Conn::Bot:  return false;
    case Conn::Not:  return contains(bf->get_sub(), v);
    case Conn::And:
    case Conn::Or:
    {
        for (const Bf_ptr& s : bf->get_subs())
            if (contains(s, v))
                return true;
        return false;
    }
    }
    assert(false);
}

bool evaluate (const Bf_ptr& bf, const Bv& val)
{
    switch (bf->t)
    {
    case Conn::Base:
        assert(0 <= bf->get_int() && bf->get_int() < static_cast<int>(val.len()));
        return val[bf->get_int()];
    case Conn::Top:  return true;
    case Conn::Bot:  return false;
    case Conn::Not:  return !evaluate(bf->get_sub(), val);
    case Conn::And:
    {
        for (const Bf_ptr& s : bf->get_subs())
            if (!evaluate(s, val))
                return false;
        return true;
    }
    case Conn::Or:
    {
        for (const Bf_ptr& s : bf->get_subs())
            if (!evaluate(s, val))
                return true;
        return false;
    }
    }
    assert(false);
}

string tabulate (const Bf_ptr& bf, size_t len)
{
    string table = "listing truth table:\n"s;
    size_t line_size = len + " 0\n"s.size(),
           line_num  = pow(2, len);
    table.reserve(table.size() + line_size * line_num);

    for (unsigned i=0; i<pow(2,len); i++)
    {
        auto tmp = Bv{len, i};
        table += tmp.to_string();
        table += ' ';

        auto ret = evaluate(bf, tmp);

        table += to_string(static_cast<unsigned>(ret));
        table += '\n';
    }

    return table;
}
