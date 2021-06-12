
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

#include "direct_types.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "more_mana.hxx"
#include "bf_to_minisat.hxx"
#include "aig_to_minisat.hxx"

namespace Direct
{
//=================================================================================================
// Naive eventually consistent teacher
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

    bool degen (); // if init intersects bad

    Bv minimize (const Bv& bv, const Face& face); // minimize `bv` in `face`
    // counterexample populator
    Feedback consider (const vector<Face>& faces); // add `hypts` and check if invariant is found
    bool aligned (const Face& face); // if `face` prevents `hypt` from being transitive

    Bv counterexample () const;
    const Feedback& check_state () const;

private:
    Mana m;
    Aig aig;
    map<int,int> curr_index_varmap, next_index_varmap;

    Sw sw;
    // Working with bf formulae
    //
    // formulae for given conditions
    Bf_ptr init,      // Init(I,X)
           bad,       // Bad(I,X)
           trans;     // Trans(I,X,X')
    // formulae for hypotheses to be tested
    Bf_ptr hypt,      // Hypt(X)
           hyptp;     // Hypt(X')

    Feedback state = Unknown;
    Bv ce = Bv{};
    // Learning apparatuses
    //
    void     setup  (); // prepare learning environment
    Feedback judge  (const vector<Face>& faces); // heuristic to determine feedback
};
//=================================================================================================
}
