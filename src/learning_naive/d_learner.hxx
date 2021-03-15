
#pragma once

#include <cassert>

#include <vector>
using std::vector;

#include "face.hxx"
#include "bv.hxx"
#include "bf.hxx"
#include "to_bf.hxx"
#include "more_bf.hxx"

#include "d_types.hxx"
#include "d_teacher.hxx"

//=================================================================================================
// Naive greedy persistent learner
//
struct D_Learner
{
    using Feedback = D_Types::Feedback;
    // using enum D_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    D_Learner  (D_Teacher& teacher_);
    ~D_Learner () = default;

    void learn ();
    const Feedback& result () const;

private:
    D_Teacher& teacher;
    Feedback fb = Unknown;
    Bv ce;

    vector<Face> hypts; // cdnf
};
