
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
    case Conn::Base:
    {
        assert(m.nVars() > bf->get_int());
        return bf->get_int();
    }
    case Conn::Not:
    {
        Var v = m.newVar(false); // tseitin nodes
        Var sub_v = addBf(bf->get_sub(), m, sw);
        m.addClause(sw,  mkLit(v),  mkLit(sub_v));
        m.addClause(sw, ~mkLit(v), ~mkLit(sub_v));
        return v;
    }
    case Conn::And:
    {
        Var v = m.newVar(false); // tseitin nodes
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
        Var v = m.newVar(false); // tseitin nodes
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

//=================================================================================================
// Aig => Minisat
//
namespace
{
    inline void validate_varmap (const map<AigVar,Var>& varmap, const Aig& aig)
    {
        // every nodes are mapped
        for (size_t i=0; i<1+aig.num_inputs()+aig.num_latches()+aig.num_ands(); i++)
            assert(varmap.contains(i));
    }
}
map<AigVar,Var> addAig (const Aig& aig, Mana& m)
{
    map<AigVar,Var> varmap; // mapping from AigVar(unsigned) to Var(int)

    // when AigVar has var 0 it is a constant (0=false, 1=true)
    varmap[0] = m.constFalse();

    // add input
    for (const auto& aigvar : aig.inputs())
    {
        assert(aigvar > 0);               // must not be constant
        assert(!varmap.contains(aigvar)); // must be fresh number

        varmap[aigvar] = m.newVar();
    }

    // add state
    for (const auto& [aigvar,aiglit] : aig.latches())
    {
        assert(aigvar > 0);               // must not be constant
        assert(!varmap.contains(aigvar)); // must be fresh number

        varmap[aigvar] = m.newVar();
    }

    // add and
    for (auto bmap = toBfmap(varmap); const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        assert(aigvar > 0);               // must not be constant
        assert(!varmap.contains(aigvar)); // must be fresh number
        assert(varmap.contains(aiglit1.var) && // assuming ordered
               varmap.contains(aiglit2.var));

        Bf_ptr tmpbf = subst(toBf(aiglit1) & toBf(aiglit2), bmap);
        bmap[aigvar] = varmap[aigvar] = addBf(tmpbf, m);
    }

    validate_varmap(varmap, aig);
    return varmap;
}

//=================================================================================================
// Stats
//
size_t dryCountClause (const Bf_ptr& bf)
{
    switch (bf->t)
    {
    case Conn::Top:  [[fallthrough]];
    case Conn::Bot:  [[fallthrough]];
    case Conn::Base: return 0;
    case Conn::Not:  return 2 + dryCountClause(bf->get_sub());
    case Conn::And:  [[fallthrough]];
    case Conn::Or:
    {
        size_t tmp_sum = 0;
        for (Bf_ptr sub : bf->get_subs())
            tmp_sum += (1 + dryCountClause(sub));
        return 1 + tmp_sum;
    }
    }
    throw InvalidBfConn("Unmatched cases."s);
}

size_t dryCountClause (const Aig& aig)
{
    size_t sum = 0;
    for (const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        Bf_ptr tmpbf = toBf(aiglit1) & toBf(aiglit2);
        sum += dryCountClause(tmpbf);
    }
    return sum;
}
