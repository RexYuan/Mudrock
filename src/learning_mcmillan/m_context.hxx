
#pragma once

#include <cassert>

#include "m_types.hxx"
#include "m_teacher.hxx"
#include "m_learner.hxx"

//=================================================================================================
// Unrolling reset dispatch
//
struct M_Context
{
    using Feedback = M_Types::Feedback;
    // using enum D_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    M_Context  (string filename);
    ~M_Context () = default;

    Feedback check ();

    bool sat () const; // if there's a path from init to bad

//private:
    M_Teacher teacher;
    M_Learner learner;

    Feedback result = Unknown;
};
