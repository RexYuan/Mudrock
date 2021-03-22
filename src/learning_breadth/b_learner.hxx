
#pragma once

#include <cassert>

#include <vector>
using std::vector;

#include "face.hxx"
#include "bv.hxx"
#include "bf.hxx"
#include "to_bf.hxx"
#include "more_bf.hxx"

#include "b_types.hxx"
#include "b_teacher.hxx"

//=================================================================================================
// Like M_Learner but distinguishing between initial/restart and persisted learning
//
struct B_Learner
{
    using Feedback = B_Types::Feedback;
    // using enum B_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    B_Learner  (B_Teacher& teacher_);
    ~B_Learner () = default;

    void kickoff ();
    void learn ();
    void clear ();
    inline void relearn () { clear(); learn(); }

    const Feedback& result () const;

private:
    B_Teacher& teacher;
    Feedback fb = Unknown;
    Bv ce = Bv{};

    Bv minimize (const Bv& ce, const Face& f);

    vector<Face> hypts; // cdnf
};
