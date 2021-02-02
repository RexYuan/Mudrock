
#pragma once

#include <cassert>

#include <set>

#include "minisat.hxx"

using std::set;
using std::same_as;

using namespace Minisat;
using LitVec = vec<Lit>;

//=================================================================================================
// Minisat manager
//
struct Mana
{
    //=============================================================================================
    // Switch management
    //
    Var newSw (); // new switches default to active
    void releaseSw (Var sw);
    void activateSw (Var sw);
    void deactivateSw (Var sw);
    
    //=============================================================================================
    // External solver interface
    //
    Var newVar (bool dvar = true); // default is decision variable
    void releaseVar (Var v);

    template <typename... Ts> requires (same_as<Ts, Lit> && ...)
    bool addClause (Var sw, Ts... lits);
    bool addClause (Var sw, const LitVec& ps);

    template <typename... Ts> requires (same_as<Ts, Lit> && ...)
    bool solve (Ts... lits); // solve assuming lits
    bool solve (const LitVec& ps); // solve assuming ps

    bool sat () const;
    bool val (Var x) const;
    int nVars () const;

private:
    Solver s;

    set<Var> activeSw;
    set<Var> inactiveSw;

    //=============================================================================================
    // Internal solver interface
    //
    inline Var _newVar (lbool upol, bool dvar)
        { return s.newVar(upol, dvar); }
    inline void _releaseVar (Lit l)
        { return s.releaseVar(l); }
    inline bool _addClause (LitVec& ps)
        { return s.addClause_(ps); }
    inline bool _solve (LitVec& ps)
        { return s.solve(ps); }
    inline bool _okay () const
        { return s.okay(); }
    inline lbool _value (Var x) const
        { return s.value(x); }
    inline lbool _modelValue (Var x) const
        { return s.modelValue(x); }
    inline int _nVars () const
        { return s.nVars(); }
};

template <typename... Ts> requires (same_as<Ts, Lit> && ...)
inline bool Mana::addClause (Var sw, Ts... lits)
{
    LitVec ps; (ps.push(lits), ...);
    return addClause(sw, ps);
}

template <typename... Ts> requires (same_as<Ts, Lit> && ...)
inline bool Mana::solve (Ts... lits)
{
    LitVec ps; (ps.push(lits), ...);
    return solve(ps);
}
