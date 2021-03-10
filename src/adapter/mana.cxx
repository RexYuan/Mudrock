
#include "mana.hxx"

Mana::Mana () : _constTrue{s.newVar()}, _constFalse{s.newVar()}
{
    s.addClause(mkLit(_constTrue));
    s.addClause(~mkLit(_constFalse));
}

// Constants
//
const Var Mana::constTrue  () const { return _constTrue; }
const Var Mana::constFalse () const { return _constFalse; }
const Sw  Mana::fixedSw    () const { return _constFalse; }

//=================================================================================================
// Switch management
//
const set<Sw>& Mana::activeSw   () const { return _activeSw; }
const set<Sw>& Mana::inactiveSw () const { return _inactiveSw; }

Sw Mana::newSw (bool active)
{
    Sw sw = s.newVar();
    if (active) _activeSw.insert(sw);
    else _inactiveSw.insert(sw);
    return sw;
}

void Mana::releaseSw (Sw sw)
{
    assert(sw != fixedSw());
    assert(sw < nVars());
    assert(activeSw().contains(sw) || inactiveSw().contains(sw));

    _activeSw.erase(sw);
    _inactiveSw.erase(sw);
    s.addClause(mkLit(sw)); 
}

void Mana::activateSw (Sw sw)
{
    _inactiveSw.erase(sw);
    _activeSw.insert(sw);
}

void Mana::deactivateSw (Sw sw)
{
    _activeSw.erase(sw);
    _inactiveSw.insert(sw);
}

//=================================================================================================
// External solver interface
//
Var Mana::newVar (bool decision)
{
    return s.newVar(l_Undef, decision);
}

void Mana::releaseVar (Var v)
{
    assert (s.value(v) == l_Undef);
    return s.releaseVar(mkLit(v));
}

int Mana::nVars () const
{
    return s.nVars();
}

bool Mana::addClause (Sw sw, const vec<Lit>& ps)
{
    vec<Lit> tmp; ps.copyTo(tmp);
    if (sw != fixedSw()) // no need to add switch with fixedSw
        tmp.push(mkLit(sw));
    return s.addClause_(tmp);
}

bool Mana::solve (const vec<Lit>& ps)
{
    vec<Lit> tmp; ps.copyTo(tmp);
    for (auto sw :   activeSw()) tmp.push(~mkLit(sw));
    for (auto sw : inactiveSw()) tmp.push( mkLit(sw));
    return s.solve(tmp);
}

bool Mana::ok () const
{
    return s.okay();
}

bool Mana::val (Var x) const
{
    assert (s.modelValue(x) != l_Undef);
    assert (s.okay());
    assert (!activeSw().contains(x) && !inactiveSw().contains(x));
    return (s.modelValue(x) == l_True);
}
