
#include "mana.hxx"

Mana::Mana ()
{
    _constTrue = s.newVar(); _constFalse = s.newVar();
    s.addClause(mkLit(_constTrue)); s.addClause(~mkLit(_constFalse));
}

// Constants
//
Var Mana::constTrue () { return _constTrue; }
Var Mana::constFalse () { return _constFalse; }
Var Mana::fixedSw () { return _constFalse; }

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

void Mana::releaseAll ()
{
    for (auto s : activeSw) releaseSw(s);
    for (auto s : inactiveSw) releaseSw(s);
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

bool Mana::addClause (Var sw, const vec<Lit>& ps)
{
    vec<Lit> tmp; ps.copyTo(tmp);
    tmp.push(mkLit(sw));
    return _addClause(tmp);
}

bool Mana::solve (const vec<Lit>& ps)
{
    vec<Lit> tmp; ps.copyTo(tmp);
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
