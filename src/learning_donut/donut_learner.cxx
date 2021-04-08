
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
    auto query = [&](const vector<Face>& faces) -> Feedback
    {
        Feedback ret;
        pause(prof.learner_total);

        ret = teacher.consider(hypts);

        resume(prof.learner_total);
        return ret;
    };
    start(prof.learner_total);

    assert(hypts.empty()); // call clear() before re-use

    fb = query(hypts); // empty conj degens to true
    if (fb == Perfect) // bad is empty
        goto ret;
    assert(fb == TooBig);

    ce = teacher.counterexample(); // get a neg ce
    hypts.push_back(Face{ce}); // init the first dnf
    fb = query(hypts); // empty disj degens to false
    if (fb == Perfect) // init is empty
        goto ret;
    assert(fb == TooSmall);

    while (true)
    {
        switch (fb)
        {
            // needs g++-11
            case Perfect: goto ret; // we're done
            case Refuted: [[fallthrough]];
            case Unknown: assert(false); // shouldn't happen
            case TooBig:
            {
                start(prof.toobig_time);

                ce = teacher.counterexample(); // negative ce
                assert(ce);
                hypts.push_back(Face{ce}); // add a new councillor

                stop(prof.toobig_time);
                break;
            }
            case TooSmall:
            {
                start(prof.toosmall_time);

                ce = teacher.counterexample(); // positive ce
                for (auto& hypt : hypts)
                {
                    if (!hypt(ce))
                    {
                        ce = minimize(ce, hypt);
                        hypt.push(ce); // augment councillor
                    }
                }

                stop(prof.toosmall_time);
                break;
            }
        }
        fb = query(hypts);
    }

ret:
    stop(prof.learner_total);
    return;
}

Bv Learner::minimize (const Bv& ce, const Face& f)
{
    auto query = [&](const Bv& bv) -> bool
    {
        bool ret;
        pause(prof.learner_total, prof.toosmall_time);

        ret = teacher.consider(bv);

        resume(prof.learner_total, prof.toosmall_time);
        return ret;
    };

    Bv tmp = ce;
    for (size_t i=0; i<ce.len(); i++)
    {
        if (tmp[i] != f.basis()[i])
        {
            tmp.flipper(i);
            if (query(tmp))
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
