
#pragma once

#include <cassert>

#include <vector>
using std::vector;

#include "face.hxx"
#include "bv.hxx"
#include "bf.hxx"
#include "more_bf.hxx"

#include "direct_types.hxx"
#include "direct_teacher.hxx"

namespace Direct
{
//=================================================================================================
// Naive greedy persistent learner
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
    const Feedback& result () const;

private:
    Teacher& teacher;
    Feedback fb = Unknown;
    Bv_ptr ce{nullptr};

    vector<Face> hypts; // cdnf
};
//=================================================================================================
}
