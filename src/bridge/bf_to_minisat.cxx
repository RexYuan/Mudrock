
#include "bf_to_minisat.hxx"

//=================================================================================================
// Bf => Minisat
//
Var addBf (const Bf_ptr& bf, Mana& m, optional<Sw> swopt, optional<bool> dvaropt)
{
    Sw sw = swopt ? swopt.value() : m.fixedSw();
    bool dvar = dvaropt ? dvaropt.value() : false; // defaults to non-decision var
    switch (bf->t)
    {
    case Conn::Top:  return m.constTrue();
    case Conn::Bot:  return m.constFalse();
    case Conn::Base:
    {
        assert(m.nVars() > bf->get_int());
        return bf->get_int();
    }
    case Conn::Not:
    {
        Var v = m.newVar(dvar); // tseitin nodes
        Var sub_v = addBf(bf->get_sub(), m, sw);
        m.addClause(sw,  mkLit(v),  mkLit(sub_v));
        m.addClause(sw, ~mkLit(v), ~mkLit(sub_v));
        return v;
    }
    case Conn::And:
    {
        Var v = m.newVar(dvar); // tseitin nodes
        vec<Lit> ps; ps.push(mkLit(v));
        for (Var sub_v; Bf_ptr sub : bf->get_subs())
        {
            sub_v = addBf(sub, m, sw);
            m.addClause(sw, ~mkLit(v), mkLit(sub_v));
            ps.push(~mkLit(sub_v));
        }
        m.addClause(sw, ps);
        return v;
    }
    case Conn::Or:
    {
        Var v = m.newVar(dvar); // tseitin nodes
        vec<Lit> ps; ps.push(~mkLit(v));
        for (Var sub_v; Bf_ptr sub : bf->get_subs())
        {
            sub_v = addBf(sub, m, sw);
            m.addClause(sw, mkLit(v), ~mkLit(sub_v));
            ps.push(mkLit(sub_v));
        }
        m.addClause(sw, ps);
        return v;
    }
    }
    throw InvalidBfConn("Unmatched cases."s);
}

void fixBf (const Bf_ptr& bf, Mana& m)
{
    Var tmp = addBf(bf, m);
    [[maybe_unused]] bool ret = m.addClause(m.fixedSw(), mkLit(tmp));
    assert(ret);
}
