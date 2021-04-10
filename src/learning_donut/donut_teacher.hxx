
#pragma once

#include <cassert>

#include <string>
using std::string;
#include <map>
using std::map;
#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include "donut_types.hxx"
#include "donut_profiler.hxx"
#include "donut_logger.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "to_minisat.hxx"

namespace Donut
{
//=================================================================================================
// Classic Teacher for stepwise over/under-approximation with unrolling
//
struct Teacher : virtual public Profiled
{
    // using enum Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    Teacher  (const string& filename);
    ~Teacher () = default;

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
    bool consider (Bv bv);
    // counterexample populators
    //
    Feedback consider (const vector<Face>& faces); // if frontier image < `faces` < bad

    Bv counterexample () const;
    const Feedback& check_state () const;

    inline const TeacherProfiler& get_prof () const { return prof; }

private:
    Mana m;
    Aig aig;
    map<int,int> first_aig_varmap,   second_aig_varmap,   last_aig_varmap;
    map<int,int> first_index_varmap, second_index_varmap, last_index_varmap;

    Sw sw;
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

    Feedback state = Unknown;
    Bv ce = Bv{};

    TeacherProfiler prof;
};
//=================================================================================================
}
