
#pragma once

#include <cassert>

#include "direct_types.hxx"
#include "direct_teacher.hxx"
#include "direct_learner.hxx"

namespace Direct
{
//=================================================================================================
// Naive learning dispatch context
//
struct Context
{
    // using enum Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    Context  (string filename);
    ~Context () = default;

    void check ();

    bool sat () const; // if there's a path from init to bad

//private:
    Teacher teacher;
    Learner learner;

    Feedback result = Unknown;
};
//=================================================================================================
}
