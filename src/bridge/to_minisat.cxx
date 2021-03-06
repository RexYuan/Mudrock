
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

//=================================================================================================
// Aig => Minisat
//
namespace
{
    // every single
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
    varmap[0] = var_Undef;

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
        // add previous state here?

        varmap[aigvar] = m.newVar();
    }

    // add and
    for (const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        assert(aigvar > 0);               // must not be constant
        assert(!varmap.contains(aigvar)); // must be fresh number
        assert(varmap.contains(aiglit1.var) && // assuming ordered
               varmap.contains(aiglit2.var));

        Lit parent = mkLit(varmap[aigvar] = m.newVar(false)), // tseitin nodes
            left   = aiglit1.sign ? mkLit(varmap[aiglit1.var]) :
                                   ~mkLit(varmap[aiglit1.var]),
            right  = aiglit2.sign ? mkLit(varmap[aiglit2.var]) :
                                   ~mkLit(varmap[aiglit2.var]);

        m.addClause(m.fixedSw(), ~parent, left);
        m.addClause(m.fixedSw(), ~parent, right);
        m.addClause(m.fixedSw(), ~left, ~right, parent);
    }

    validate_varmap(varmap, aig);
    return varmap;
}
