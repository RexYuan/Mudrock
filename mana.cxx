
#include "mana.hxx"

//=================================================================================================
// Switch management
//
Var Mana::newSw ()
{
    Var sw=s.newVar();
    activeSw.insert(sw);
    return sw;
}

void Mana::releaseSw (Var sw)
{
    assert (sw < nVars());
    assert (activeSw.contains(sw) || inactiveSw.contains(sw));
    activeSw.erase(sw);
    inactiveSw.erase(sw);
    s.addClause(mkLit(sw)); 
}

void Mana::activateSw (Var sw)
{
    inactiveSw.erase(sw);
    activeSw.insert(sw);
}

void Mana::deactivateSw (Var sw)
{
    activeSw.erase(sw);
    inactiveSw.insert(sw);
}

//=================================================================================================
// External solver interface
//
Var Mana::newVar (bool dvar)
{
    return _newVar(l_Undef, dvar);
}

void Mana::releaseVar (Var v)
{
    assert (_value(v) == l_Undef);
    return _releaseVar(mkLit(v));
}

bool Mana::addClause (Var sw, const LitVec& ps)
{
    LitVec tmp; ps.copyTo(tmp);
    tmp.push(mkLit(sw));
    return _addClause(tmp);
}

bool Mana::solve (const LitVec& ps)
{
    LitVec tmp; ps.copyTo(tmp);
    for (auto sw :   activeSw) tmp.push(~mkLit(sw));
    for (auto sw : inactiveSw) tmp.push( mkLit(sw));
    return _solve(tmp);
}

bool Mana::sat () const
{
    return _okay();
}

// return the valuation of `x` in satisfied model
bool Mana::val (Var x) const
{
    assert (_modelValue(x) != l_Undef);
    assert (_okay());
    assert (!activeSw.contains(x) && !inactiveSw.contains(x));
    return (_modelValue(x) == l_True);
}

int Mana::nVars () const
{
    return _nVars();
}
