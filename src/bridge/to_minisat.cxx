
#include "to_minisat.hxx"

//=================================================================================================
// Bf => Minisat
//
Var addBf (const Bf_ptr& bf, Mana& m, optional<Sw> swopt)
{
    Sw sw = swopt ? swopt.value() : m.fixedSw();
    switch (bf->t)
    {
    case Conn::Top:  return m.constTrue();
    case Conn::Bot:  return m.constFalse();
    case Conn::Base: return bf->get_int();
    case Conn::Not:
    {
        Var v = m.newVar(false);
        Var sub_v = addBf(bf->get_sub(), m, sw);
        m.addClause(sw,  mkLit(v),  mkLit(sub_v));
        m.addClause(sw, ~mkLit(v), ~mkLit(sub_v));
        return v;
    }
    case Conn::And:
    {
        Var v = m.newVar();
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
        Var v = m.newVar(false);
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
    assert(false);
}

bool sat (const Bf_ptr& bf, Mana& m)
{
    Var tmpSw = m.newSw();
    Var bfVar = addBf(bf, m, tmpSw);
    bool ret = m.solve(mkLit(bfVar));
    m.releaseSw(tmpSw);
    return ret;
}

bool hold (const Bf_ptr& bf, Mana& m)
{
    return !sat(~bf, m);
}
