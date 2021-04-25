
#pragma once

#include <cassert>

#include "sig.hxx"

#include <vector>
using std::vector;

#include "face.hxx"
#include "bv.hxx"
#include "bf.hxx"
#include "to_bf.hxx"
#include "more_bf.hxx"

#include "donut_types.hxx"
#include "donut_teacher.hxx"
#include "profiler.hxx"
#include "logger.hxx"

namespace Donut
{
//=================================================================================================
// Classic exact learner for stepwise over/under-approximation
//
struct Learner
{
    // using enum Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    Learner  (Teacher& teacher_);
    ~Learner () = default;

    void learn ();
    void clear ();
    inline void relearn () { clear(); learn(); }

    const Feedback& result () const;

private:
    Teacher& teacher;
    Feedback fb = Unknown;
    Bv ce = Bv{};

    Bv minimize (const Bv& ce, const Face& f);

    vector<Face> hypts; // cdnf
};
//=================================================================================================
}
