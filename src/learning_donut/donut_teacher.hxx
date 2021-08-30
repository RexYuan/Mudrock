
#pragma once

#include <cassert>

#include <string>
using std::string;
using namespace std::string_literals;
#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include "donut_types.hxx"
#include "profiler.hxx"
#include "logger.hxx"
#include "counting.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "bv_arena.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "more_mana.hxx"
#include "aig_to_bf.hxx"
#include "face_to_bf.hxx"
#include "bf_to_minisat.hxx"
#include "aig_to_minisat.hxx"

namespace Donut
{
//=================================================================================================
// Classic Teacher for stepwise over/under-approximation with unrolling
//
struct Teacher
{
    // using enum Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    friend struct Context;

    Teacher  (const string& filename);
    ~Teacher () = default;

    void renewMana ();

    //=============================================================================================
    // Higher order commands for context
    //
    void setup       ();           // prepare learning environment
    bool degen       ();           // if init meets bad
    bool advanceable ();           // if frontier image doesn't meet bad
    void unroll      (size_t n=1); // unroll bad by `n` step
    void restart     ();           // reset frontier back to init
    bool progressed  ();           // if current frontier > last frontier
    void advance     ();           // advance frontier

    //=============================================================================================
    // Query commands for learner
    //
    bool membership (Bv* bv);
    // counterexample populators
    //
    Feedback equivalence (const vector<Face>& faces); // if frontier image < `faces` < bad

    Bv* counterexample () const;
    const Feedback& check_state () const;

    vector<Bv> witness () const; // return refutation stimulus

private:
    Mana m;
    Aig aig;
    vector<Var> first_aig_varmap,   second_aig_varmap,   last_aig_varmap;
    vector<Var> first_state_varmap, second_state_varmap, last_state_varmap;
    vector< vector<Var> > aig_varmaps_cache;

    Sw cumu_sw, // tracking the cumulative disjuncted hypotheses set
       tent_sw; // tracking the tentative frontier hypothesis
    // Working with bf formulae
    //
    // formulae for given conditions
    Bf_ptr init,      // I(X)
           bad,       // B(X',X''...)
           trans_hd,  // T(X,X')
           trans_tl;  // T(X',X'',...)
    // formulae for hypotheses to be tested
    Bf_ptr last_frnt,  // H-1(X)  last frontier
           last_frntp, // H-1(X') last frontier
           frnt,       // H(X)    current frontier
           frntp;      // H(X')   current frontier
    // cache formula for renewing mana
    Bf_ptr f_last_frnt_cache,
           f_last_frntp_cache,
           f_frnt_cache,
           f_frntp_cache;

    size_t unroll_depth = 0;

    Feedback state = Unknown;
    Bv* ce = nullptr;
};
//=================================================================================================
}
