
#pragma once

#include <cassert>

#include <string>
using std::string;
#include <map>
using std::map;

#include <iostream>
using std::ostream;

#include "d_types.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "to_minisat.hxx"

//=================================================================================================
// Naive eventually consistent teacher
//
struct D_Teacher
{
    using Feedback = D_Types::Feedback;
    // using enum D_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    D_Teacher  (const string& filename);
    ~D_Teacher () = default;

    Feedback consider (const vector<Face>& faces); // add `hypts` and check if invariant is found
    Bv counterexample () const;
    const Feedback& check_state () const;

//private:
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
    // formulae for criteria to test hypotheses
    Bf_ptr InitCond,  // Init(I,X) => Hypt(X)
           BadCond,   // Hypt(X) => ~Bad(I,X)
           TransCond; // Hypt(X), Trans(I,X,X') => Hypt(X')

    Bv ce;
    Feedback state = Unknown;
    // Learning apparatuses
    //
    void     setup  (); // prepare learning environment
    Feedback judge  (const vector<Face>& faces); // heuristic to determine feedback
};
