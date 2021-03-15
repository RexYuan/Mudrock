
#include "d_learner.hxx"

D_Learner::D_Learner (D_Teacher& teacher_) :
teacher{teacher_},
hypts{vector<Face>{}} {}

void D_Learner::learn ()
{
    assert(hypts.empty()); // single-use learner

    if (teacher.degen())
    {
        fb = Refuted;
        return;
    }

    fb = teacher.consider(hypts); // empty conj degens to true
    if (fb == Perfect) // bad is empty
        return;
    assert(fb == TooBig);

    ce = teacher.counterexample(); // get a neg ce
    hypts.push_back(Face{ce}); // init the first dnf
    fb = teacher.consider(hypts); // empty disj degens to false
    if (fb == Perfect) // init is empty
        return;
    assert(fb == TooSmall);
}

const D_Types::Feedback& D_Learner::result () const
{
    return fb;
}
