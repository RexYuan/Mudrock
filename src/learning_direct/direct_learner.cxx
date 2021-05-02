
#include "direct_learner.hxx"

namespace Direct
{
//=================================================================================================
Learner::Learner (Teacher& teacher_) :
teacher{teacher_},
hypts{vector<Face>{}} {}

void Learner::learn ()
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

    while (true)
    {
        switch (fb)
        {
            // needs g++-11
            case Refuted: [[fallthrough]];
            case Perfect: return; // we're done
            case Unknown: assert(false); // shouldn't happen
            case TooBig:
            {
                ce = teacher.counterexample(); // negative ce
                assert(ce);
                hypts.push_back(Face{ce}); // add a new councillor
                break;
            }
            case TooSmall:
            {
                // TODO: possibility of eliminating a face by affirming its basis?
                ce = teacher.counterexample(); // positive ce
                for (auto& hypt : hypts)
                {
                    if (!hypt(ce))
                    {
                        ce = teacher.minimize(ce, hypt);
                        hypt.push(ce); // augment councillor
                        // re-align with the council while we're at it
                        while (!teacher.aligned(hypt))
                        {
                            ce = teacher.counterexample();
                            ce = teacher.minimize(ce, hypt);
                            hypt.push(ce); // positive ce
                        }
                    }
                }
                break;
            }
        }
#ifdef PRINT
        std::cout << "asking:" << "\n";
        for (auto& hypt : hypts)
            std::cout << hypt << "\n";
#endif
        fb = teacher.consider(hypts);
#ifdef PRINT
        std::cout << "feedback: " << fb << "\n";
#endif
    }
}

const Feedback& Learner::result () const
{
    return fb;
}
//=================================================================================================
}
