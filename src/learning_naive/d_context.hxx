
#pragma once

#include <cassert>

#include "d_types.hxx"
#include "d_teacher.hxx"
#include "d_learner.hxx"

//=================================================================================================
// Naive learning dispatch context
//
struct D_Context
{
    using Feedback = D_Types::Feedback;
    // using enum D_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    D_Context  (string filename);
    ~D_Context () = default;

    void check ();

    bool sat () const; // if there's a path from init to bad

//private:
    D_Teacher teacher;
    D_Learner learner;

    Feedback result = Unknown;
};
