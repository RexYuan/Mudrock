
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

#include "m_types.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "to_minisat.hxx"

//=================================================================================================
// Classic Teacher for stepwise over-approximation
//
struct M_Teacher
{
    using Feedback = M_Types::Feedback;
    // using enum M_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    M_Teacher  (const string& filename);
    ~M_Teacher () = default;

    bool degen (); // if init intersects bad

    // counterexample populator
    Feedback consider (const vector<Face>& faces); // add `hypts` and check if invariant is found

    Bv counterexample () const;
    const Feedback& check_state () const;

private:
    Mana m;
    Aig aig;
    map<int,int> first_aig_varmap, last_aig_varmap;
    map<int,int> first_index_varmap, last_index_varmap;

    Sw sw;
    // Working with bf formulae
    //
    // formulae for given conditions
    Bf_ptr init,      // Init(I,X)
           bad,       // Bad(I,X,...)
           trans;     // Trans(I,X,X',...)
    // formulae for hypotheses to be tested
    Bf_ptr hypt;      // Hypt(X)

    Feedback state = Unknown;
    Bv ce = Bv{};
    // Learning apparatuses
    //
    void setup  (); // prepare learning environment
    void unroll (); // prepare learning environment
};
