
#pragma once

#include <cassert>

#include "b_types.hxx"
#include "b_teacher.hxx"
#include "b_learner.hxx"

//=================================================================================================
// Like M_Context but learner persists after advances and only wipes at unrolls
//
struct B_Context
{
    using Feedback = B_Types::Feedback;
    // using enum B_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    B_Context  (string filename);
    ~B_Context () = default;

    Feedback check ();

    bool sat () const; // if there's a path from init to bad

//private:
    B_Teacher teacher;
    B_Learner learner;

    Feedback result = Unknown;
};
