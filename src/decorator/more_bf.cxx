
#include "more_bf.hxx"

bool contains (const Bf_ptr& bf, int v)
{
    switch (bf->get_type())
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
    throw InvalidBfConn("Unmatched cases."s);
}

Bf_ptr subst (const Bf_ptr& bf, const map<int,int>& varmap)
{
    switch (bf->t)
    {
    case Conn::Top:  return v(true);
    case Conn::Bot:  return v(false);
    case Conn::Base:
        assert(varmap.contains(bf->get_int()));
        return v(varmap.at(bf->get_int()));
    case Conn::Not:  return ~subst(bf->get_sub(), varmap);
    case Conn::And:
    {
        Bf_ptr tmp = conj();
        tmp->reserve(bf->get_subs().size());
        for (Bf_ptr s : bf->get_subs())
            tmp += subst(s, varmap);
        return tmp;
    }
    case Conn::Or:
    {
        Bf_ptr tmp = disj();
        tmp->reserve(bf->get_subs().size());
        for (Bf_ptr s : bf->get_subs())
            tmp += subst(s, varmap);
        return tmp;
    }
    }
    throw InvalidBfConn("Unmatched cases."s);
}

Bf_ptr subst (const Bf_ptr& bf, const vector<int>& varmap)
{
    switch (bf->t)
    {
    case Conn::Top:  return v(true);
    case Conn::Bot:  return v(false);
    case Conn::Base:
        assert(bf->get_int() < static_cast<int>(varmap.size()));
        return v(varmap[bf->get_int()]);
    case Conn::Not:  return ~subst(bf->get_sub(), varmap);
    case Conn::And:
    {
        Bf_ptr tmp = conj();
        tmp->reserve(bf->get_subs().size());
        for (Bf_ptr s : bf->get_subs())
            tmp += subst(s, varmap);
        return tmp;
    }
    case Conn::Or:
    {
        Bf_ptr tmp = disj();
        tmp->reserve(bf->get_subs().size());
        for (Bf_ptr s : bf->get_subs())
            tmp += subst(s, varmap);
        return tmp;
    }
    }
    throw InvalidBfConn("Unmatched cases."s);
}

bool evaluate (const Bf_ptr& bf, const Bv_ptr val)
{
    switch (bf->get_type())
    {
    case Conn::Base:
        assert(0 <= bf->get_int() && bf->get_int() < static_cast<int>(val->len()));
        return (*val)[bf->get_int()];
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
    throw InvalidBfConn("Unmatched cases."s);
}

string tabulate (const Bf_ptr& bf, size_t len)
{
    string table = "listing truth table:\n"s;
    size_t line_size = len + " 0\n"s.size(),
           line_num  = pow(2, len);
    table.reserve(table.size() + line_size * line_num);

    for (unsigned i=0; i<pow(2,len); i++)
    {
        auto tmp = mkBv(len, i);
        table += tmp->to_string();
        table += ' ';

        auto ret = evaluate(bf, tmp);

        table += to_string(static_cast<unsigned>(ret));
        table += '\n';
    }

    return table;
}
