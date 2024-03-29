
#pragma once

#include <cassert>

#include <string>
using std::string;
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
#include "aig_to_bf.hxx"
#include "bv_to_bf.hxx"
#include "face_to_bf.hxx"
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

    Bv_ptr minimize (const Bv_ptr bv, const Face& face); // minimize `bv` in `face`
    // counterexample populator
    Feedback consider (const vector<Face>& faces); // add `hypts` and check if invariant is found
    bool aligned (const Face& face); // if `face` prevents `hypt` from being transitive

    Bv_ptr counterexample () const;
    const Feedback& check_state () const;

private:
    Mana m;
    Aig aig;
    vector<Var> curr_state_varmap, next_state_varmap;

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
    Bv_ptr ce{nullptr};
    // Learning apparatuses
    //
    void     setup  (); // prepare learning environment
    Feedback judge  (const vector<Face>& faces); // heuristic to determine feedback
};
//=================================================================================================
}
