
#include "aig_to_minisat.hxx"

//=================================================================================================
// Aig => Minisat
//
namespace
{
    inline void validate_varmap (const vector<Var>& varmap, const Aig& aig, const Mana& m)
    {
        assert(varmap.size() == 1+aig.maxvar());
        // every AigVar has a mapped Var
        for (size_t i=0; i<1+aig.maxvar(); i++)
        {
            assert(varmap[i] != var_Undef);
            assert(varmap[i] < m.nVars());
        }
    }

    // must be valid within range of map
    inline bool isValid (AigVar aigvar, const vector<Var>& varmap)
    {
        return aigvar < varmap.size();
    }

    // must be un-newVar-ed fresh number
    inline bool isFresh (AigVar aigvar, const vector<Var>& varmap)
    {
        return varmap[aigvar] == var_Undef;
    }

    // ready to be newVar-ed
    inline bool isNew (AigVar aigvar, const vector<Var>& varmap)
    {
        return !isConst(aigvar) &&
               isValid(aigvar, varmap) &&
               isFresh(aigvar, varmap);
    }

    // children precede parent
    inline bool isOrdered (AigVar aigvar, AigLit aiglit1, AigLit aiglit2,
                           const vector<Var>& varmap)
    {
        return !isFresh(aiglit1.var, varmap) &&
               !isFresh(aiglit2.var, varmap);
    }
}
vector<Var> addAig (const Aig& aig, Mana& m)
{
    vector<Var> varmap(1+aig.maxvar(), var_Undef); // index AigVar(unsigned) => Var(int)

    // AigVar 0 is constant bools (0=false, 1=true)
    varmap[0] = m.constFalse();

    // add input
    for (const auto& aigvar : aig.inputs())
    {
        assert(isNew(aigvar, varmap));
        varmap[aigvar] = m.newVar();
    }

    // add state
    for (const auto& [aigvar,aiglit] : aig.latches())
    {
        assert(isNew(aigvar, varmap));
        varmap[aigvar] = m.newVar();
    }

    // add and
    for (const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        assert(isNew(aigvar, varmap));
        assert(isOrdered(aigvar, aiglit1, aiglit2, varmap));
        Bf_ptr tmpbf = subst(toBf(aiglit1) & toBf(aiglit2), varmap);
        varmap[aigvar] = addBf(tmpbf, m);
    }

    validate_varmap(varmap, aig, m);
    return varmap;
}

vector<Var> extendAig (const vector<Var>& last_varmap, const Aig& aig, Mana& m)
{
    vector<Var> varmap(1+aig.maxvar(), var_Undef);

    // when AigVar has var 0 it is a constant (0=false, 1=true)
    varmap[0] = m.constFalse();

    // add input
    for (const auto& aigvar : aig.inputs())
    {
        assert(isNew(aigvar, varmap));
        varmap[aigvar] = m.newVar();
    }

    // add state in terms of last step
    for (const auto& [aigvar,aiglit] : aig.latches())
    {
        assert(isNew(aigvar, varmap));
        varmap[aigvar] = addBf(subst(toBf(aiglit), last_varmap), m);
    }

    // add and
    for (const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        assert(isNew(aigvar, varmap));
        assert(isOrdered(aigvar, aiglit1, aiglit2, varmap));
        // set decision if it's next state
        bool is_next = false;
        for (const auto& [curr_aigvar,next_aiglit] : aig.latches())
            if (aigvar == next_aiglit.var) is_next = true;

        Bf_ptr tmpbf = subst(toBf(aiglit1) & toBf(aiglit2), varmap);
        varmap[aigvar] = addBf(tmpbf, m, nullopt, is_next);
    }

    validate_varmap(varmap, aig, m);
    return varmap;
}
