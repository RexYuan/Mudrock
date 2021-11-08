
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
struct MemberMana
{
    MemberMana (const Aig& aig);
    void setup ();
    void renew ();
    void restart ();
    void advance (Bf_ptr cdnf);

    bool membership (const Bv_ptr bv); // if `bv` is image of `f_cumu_hypt`

    Mana m;
    const Aig& aig;
    vector<Var> first_state_varmap, second_state_varmap; // state var to minisat var

    Bf_ptr f_init, f_trans; // in solver vars
    Bf_ptr f_cumu_hypt; // in state vars
};

struct FullMana
{
    FullMana (const Aig& aig);
    void setup ();
    void renew ();
    void restart ();
    void advance ();

    bool progress (Bf_ptr cdnf); // if `cdnf` is forward img overapprox of `f_cumu_hypt`
                                 // last H(X), T(X,X') => H(X')
    bool fixedpoint (); // if `frnt` is in `f_cumu_hypt`
                        // H(X) => last H(X) and H is non-empty

    Bv_ptr get_ce (); // ce for progress

    Mana m;
    const Aig& aig;
    vector<Var> first_state_varmap, second_state_varmap; // state var to minisat var

    Bf_ptr frnt, frntp;
    Bf_ptr f_init;
    Bf_ptr cumu_hypt;

    Bv_ptr ce;
    Sw tent_sw, cumu_sw;
};

struct CoiMana
{
    CoiMana (const Aig& aig);
    void setup ();
    void renew ();
    void restart ();
    void advance ();
    void unroll (size_t n);

    bool reachbad (); // if init reaches bad
    bool meetbad  (); // if frontier image doesn't meet bad
    bool soundness (Bf_ptr cdnf); // H(X'), T(X',X'',...) => ~B(X',X'',...)

    Bv_ptr get_ce (); // ce for progress

    Mana m;
    const Aig& aig;
    vector<Var> first_aig_varmap,   second_aig_varmap,   last_aig_varmap;
    vector<Var> first_state_varmap, second_state_varmap, last_state_varmap; // state var to minisat var
    vector< vector<Var> > aig_varmaps_cache; // for witness extraction

    Bf_ptr frnt, frntp;
    Bf_ptr init, badp;
    Bf_ptr cumu_hypt;

    Bf_ptr trans_hd, trans_tl;

    Bv_ptr ce;
    Sw tent_sw, cumu_sw;

    size_t unroll_depth{0};
};

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

    //=============================================================================================
    // Higher order commands for context
    //
    void setup       ();           // prepare learning environment
    void unroll      (size_t n=1); // unroll bad by `n` step
    void restart     ();           // reset frontier back to init
    void advance     ();           // advance frontier

    bool degen      (); // if init meets bad
    bool reachbad   (); // if init reaches bad
    bool meetbad    (); // if frontier image doesn't meet bad
    bool fixedpoint (); // if current frontier > last frontier

    //=============================================================================================
    // Query commands for learner
    //
    bool membership (const Bv_ptr bv);
    // counterexample populators
    Feedback equivalence (const vector<Face>& faces); // if frontier image < `faces` < b

    Bv_ptr counterexample () const;
    const Feedback& check_state () const;

    vector<Bv_ptr> witness () const; // return refutation stimulus

private:
    Aig aig;

    MemberMana mm;
    FullMana fm;
    CoiMana cm;

    Bf_ptr cdnf_cache;

    Feedback state = Unknown;
    Bv_ptr ce{nullptr};
};
//=================================================================================================
}
