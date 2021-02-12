
#pragma once

#include <cassert>

#include <vector>
#include <string>

#include <functional>
#include <optional>

#include "minisat.hxx"

#include "mana.hxx"
#include "bf.hxx"
#include "bv.hxx"

using std::vector;
using std::string;
using std::function;
using std::optional; using std::nullopt;

using namespace Minisat;

// Transition system
//
using State = vector<Var>;

//=================================================================================================
// Algorithm oracle
//
struct Ora
{
    Ora (size_t length);

    //=============================================================================================
    // Main interface
    //
    bool sat (const Bf_ptr& bf, optional<vector<size_t>> space=nullopt);
    bool hold (const Bf_ptr& bf, optional<vector<size_t>> space=nullopt);

    void assume (const Bf_ptr& bf, optional<vector<size_t>> space=nullopt); // nulled after refresh
    void insist (const Bf_ptr& bf, optional<vector<size_t>> space=nullopt); // nulled after reset
    void persist (const Bf_ptr& bf, optional<vector<size_t>> space=nullopt); // fixed

    bool eval (const Bv& bv, optional<vector<size_t>> bv_space=nullopt);
    bool eval (const Bv& bv, const Bf_ptr& bf,
               optional<vector<size_t>> bv_space=nullopt,
               optional<vector<size_t>> bf_space=nullopt);

    //=============================================================================================
    // Command
    //
    void refresh (); // forget working memory
    void reset (); // forget everything

    //=============================================================================================
    // State management
    //
    size_t nStates ();
    size_t addState ();

    //=============================================================================================
    // Stats
    //
    string tabulation ();

private:
    size_t l;
    Mana m;
    Var primarySw, secondarySw;
    vector<State> states;

    //=============================================================================================
    // Internal utilities
    //
    Var importBf (Var sw, const Bf_ptr& bf, optional<vector<size_t>> spaceopt);
    Var Ind2Var (int ind, optional<vector<size_t>> spaceopt);
};
