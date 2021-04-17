
#include "donut_learner.hxx"

#define SETN string(__FILE__) + " algo"s
#define PROF_ALGO(name, code...) \
    log(2, SETN, "Began ", #name); \
    SingletonProfiler::Start(SETN, #name); \
    code \
    SingletonProfiler::Stop(SETN, #name); \
    log(2, SETN "Ended ", #name, ", time spent: ", \
        to_string(SingletonProfiler::Get().get_stats(SETN, #name).laps().back()))

namespace Donut
{
//=================================================================================================
Learner::Learner (Teacher& teacher_) :
teacher{teacher_},
hypts{vector<Face>{}} {}

void Learner::clear ()
{
PROF_SCOPE();
    fb = Unknown;
    ce = Bv{};
    hypts.clear();
}

void Learner::learn ()
{
PROF_SCOPE();
    auto query = [&](const vector<Face>& faces) -> Feedback
    {
        Feedback ret;

        ret = teacher.consider(hypts);

        return ret;
    };

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
PROF_ALGO (toobig,
                ce = teacher.counterexample(); // negative ce
                assert(ce);
                hypts.push_back(Face{ce}); // add a new councillor
);
                break;
            }
            case TooSmall:
            {
PROF_ALGO (toosmall,
                ce = teacher.counterexample(); // positive ce
                for (auto& hypt : hypts)
                {
                    if (!hypt(ce))
                    {
                        ce = minimize(ce, hypt);
                        hypt.push(ce); // augment councillor
                    }
                }
);
                break;
            }
        }
        fb = query(hypts);
    }
ret:
    return;
}

Bv Learner::minimize (const Bv& ce, const Face& f)
{
PROF_SCOPE();
    auto query = [&](const Bv& bv) -> bool
    {
        bool ret;
        SingletonProfiler::Pause(SETN, "toosmall");

        ret = teacher.consider(bv);

        SingletonProfiler::Resume(SETN, "toosmall");
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

#undef PROF_ALGO
#undef SETN
