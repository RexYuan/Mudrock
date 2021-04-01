
#include "donut_learner.hxx"

namespace Donut
{
//=================================================================================================
Learner::Learner (Teacher& teacher_) :
teacher{teacher_},
hypts{vector<Face>{}} {}

void Learner::clear ()
{
    fb = Unknown;
    ce = Bv{};
    hypts.clear();
}

void Learner::learn ()
{
    //assert(hypts.empty()); // call clear() before re-use

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
            case Perfect: return; // we're done
            case Refuted: [[fallthrough]];
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
                ce = teacher.counterexample(); // positive ce
                for (auto& hypt : hypts)
                {
                    if (!hypt(ce))
                    {
                        ce = minimize(ce, hypt);
                        hypt.push(ce); // augment councillor
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

Bv Learner::minimize (const Bv& ce, const Face& f)
{
    Bv tmp = ce;
    for (size_t i=0; i<ce.len(); i++)
    {
        if (tmp[i] != f.basis()[i])
        {
            tmp.flipper(i);
            if (teacher.consider(tmp))
                i = 0;
            else
                tmp.flipper(i);
        }
    }

    return tmp;
}

const Feedback& Learner::result () const
{
    return fb;
}
//=================================================================================================
}
