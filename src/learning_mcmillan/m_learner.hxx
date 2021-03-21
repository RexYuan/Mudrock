
#pragma once

#include <cassert>

#include <vector>
using std::vector;

#include "face.hxx"
#include "bv.hxx"
#include "bf.hxx"
#include "to_bf.hxx"
#include "more_bf.hxx"

#include "m_types.hxx"
#include "m_teacher.hxx"

//=================================================================================================
// Classic exact learner for stepwise over/under-approximation
//
struct M_Learner
{
    using Feedback = M_Types::Feedback;
    // using enum M_Types::Feedback; needs g++-11
    static const Feedback Refuted  = Feedback::Refuted;
    static const Feedback Perfect  = Feedback::Perfect;
    static const Feedback TooBig   = Feedback::TooBig;
    static const Feedback TooSmall = Feedback::TooSmall;
    static const Feedback Unknown  = Feedback::Unknown;

    M_Learner  (M_Teacher& teacher_);
    ~M_Learner () = default;

    void learn ();
    void clear ();
    inline void relearn () { clear(); learn(); }

    const Feedback& result () const;

private:
    M_Teacher& teacher;
    Feedback fb = Unknown;
    Bv ce = Bv{};

    Bv minimize (const Bv& ce, const Face& f);

    vector<Face> hypts; // cdnf
};
