
#pragma once

#include <cassert>

#include "sig.hxx"

#include "donut_types.hxx"
#include "profiler.hxx"
#include "donut_teacher.hxx"
#include "donut_learner.hxx"

namespace Donut
{
//=================================================================================================
// Unrolling reset dispatch
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

    Feedback check ();

    Feedback sat () const; // if there's a path from init to bad

private:
    Teacher teacher;
    Learner learner;

    Feedback result = Unknown;
};
//=================================================================================================
}
