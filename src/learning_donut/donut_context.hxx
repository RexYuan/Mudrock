
#pragma once

#include <cassert>

#include "donut_types.hxx"
#include "donut_profiler.hxx"
#include "donut_teacher.hxx"
#include "donut_learner.hxx"

namespace Donut
{
//=================================================================================================
// Unrolling reset dispatch
//
struct Context : virtual public Profiled
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

    bool sat () const; // if there's a path from init to bad

    inline const ContextProfiler& get_prof () const override { return prof; }

private:
    Teacher teacher;
    Learner learner;

    Feedback result = Unknown;

    ContextProfiler prof;
};
//=================================================================================================
}
