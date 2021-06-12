
#include "aig_to_minisat.hxx"

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

map<AigVar,Var> addAig (const map<AigVar,Var>& last_varmap, const Aig& aig, Mana& m)
{
    map<AigVar,Var> varmap;

    // when AigVar has var 0 it is a constant (0=false, 1=true)
    varmap[0] = m.constFalse();

    // add input
    for (const auto& aigvar : aig.inputs())
    {
        assert(aigvar > 0); // must not be constant
        varmap[aigvar] = m.newVar();
    }

    // add state in terms of last step
    for (auto bmap = toBfmap(last_varmap); const auto& [aigvar,aiglit] : aig.latches())
    {
        assert(aigvar > 0);               // must not be constant
        assert(!varmap.contains(aigvar)); // must be fresh number

        varmap[aigvar] = addBf(subst(toBf(aiglit), bmap), m);
    }

    // add and
    for (auto bmap = toBfmap(varmap); const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        assert(aigvar > 0);               // must not be constant
        assert(!varmap.contains(aigvar)); // must be fresh number
        assert(varmap.contains(aiglit1.var) && // assuming ordered
               varmap.contains(aiglit2.var));

        // set decision if it's next state
        bool is_next = false;
        for (const auto& [curr_aigvar,next_aiglit] : aig.latches())
            if (aigvar == next_aiglit.var) is_next = true;

        Bf_ptr tmpbf = subst(toBf(aiglit1) & toBf(aiglit2), bmap);
        bmap[aigvar] = varmap[aigvar] = addBf(tmpbf, m, nullopt, is_next);
    }

    validate_varmap(varmap, aig);
    return varmap;
}
