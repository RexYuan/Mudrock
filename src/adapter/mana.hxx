
#pragma once

#include <cassert>

#include <set>
using std::set;

#include <concepts>
using std::same_as;

#include "minisat.hxx"
using namespace Minisat;

template <typename T>     concept isLit   = same_as<T, Lit>;
template <typename... Ts> concept areLits = (isLit<Ts> && ...);

//=================================================================================================
// Minisat adapter
//
using Sw = Var;
struct Mana
{
    Mana ();
    // Minisat::Solver can't be copied or moved
    Mana           (const Mana&) = delete;
    Mana& operator=(const Mana&) = delete;
    Mana           (Mana&&) = delete;
    Mana& operator=(Mana&&) = delete;

    // Constants
    //
    const Var constTrue  () const;
    const Var constFalse () const;
    const Sw  fixedSw    () const;

    //=============================================================================================
    // Switch management
    //
    const set<Sw>& activeSw   () const;
    const set<Sw>& inactiveSw () const;

    Sw   newSw        (bool active=true);
    void releaseSw    (Sw sw);
    void activateSw   (Sw sw);
    void deactivateSw (Sw sw);

    //=============================================================================================
    // External solver interface
    //
    Var  newVar     (bool decision=true);
    void releaseVar (Var v);

    int nVars () const;

    template <typename... Ts> requires areLits<Ts...>
    bool addClause (Sw sw, Ts... lits);
    bool addClause (Sw sw, const vec<Lit>& ps);

    template <typename... Ts> requires areLits<Ts...>
    bool solve (Ts... lits);
    bool solve (const vec<Lit>& ps);

    bool ok  ()      const; // if the solve result is sat
    bool val (Var x) const; // valuation of `x` in sat model

private:
    Solver s;

    set<Sw> _activeSw;
    set<Sw> _inactiveSw;

    Var _constTrue{var_Undef}, _constFalse{var_Undef};
};

template <typename... Ts> requires areLits<Ts...>
inline bool Mana::addClause (Sw sw, Ts... lits)
{
    vec<Lit> ps; (ps.push(lits), ...);
    return addClause(sw, ps);
}

template <typename... Ts> requires areLits<Ts...>
inline bool Mana::solve (Ts... lits)
{
    vec<Lit> ps; (ps.push(lits), ...);
    return solve(ps);
}
