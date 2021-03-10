
#pragma once

#include <cassert>

#include <string>
using std::string;
#include <map>
using std::map;

#include <iostream>
using std::ostream;

#include "aig.hxx"
#include "bf.hxx"
#include "face.hxx"

#include "mana.hxx"
#include "to_minisat.hxx"

//=================================================================================================
// Naive eventually consistent teacher
//
struct D_Teacher
{
    enum class Feedback {Perfect, TooBig, TooSmall};

    D_Teacher  (const string& filename);
    ~D_Teacher () = default;

    Feedback consider (const vector<Face>& faces); // add `hypts` and check if invariant is found

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
    
    // Learning apparatuses
    //
    void     setup (); // prepare learning environment
    Feedback judge (const vector<Face>& faces); // heuristic to determine feedback
};

ostream& operator << (ostream& out, const D_Teacher::Feedback& feedback);
