
#pragma once

#include <cassert>

#include <string>
using std::string;
using namespace std::string_literals;
#include <map>
using std::map;
#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include "donut_types.hxx"
#include "profiler.hxx"
#include "logger.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "more_mana.hxx"
#include "to_minisat.hxx"
#include "to_bf.hxx"

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
    bool membership (Bv bv);
    // counterexample populators
    //
    Feedback equivalence (const vector<Face>& faces); // if frontier image < `faces` < bad

    Bv counterexample () const;
    const Feedback& check_state () const;

private:
    Mana m;
    Aig aig;
    map<int,int> first_aig_varmap,   second_aig_varmap,   last_aig_varmap;
    map<int,int> first_index_varmap, second_index_varmap, last_index_varmap;

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
    Bv ce = Bv{};
};
//=================================================================================================
}
