
#include "donut_learner.hxx"

#define PROF_METHOD(code...) \
    log(1, "Learner", "Began ", __func__); \
    SingletonProfiler::Start("learner", __func__); \
    code \
    SingletonProfiler::Stop("learner", __func__); \
    log(1, "Learner", "Ended ", __func__, ", time spent: ", \
        to_string(SingletonProfiler::Get().get_stats("learner", __func__).laps().back()))

#define PROF_ALGO(name, code...) \
    log(1, "Learner", "Began ", #name); \
    SingletonProfiler::Start("algo", #name); \
    code \
    SingletonProfiler::Stop("algo", #name); \
    log(1, "Learner", "Ended ", #name, ", time spent: ", \
        to_string(SingletonProfiler::Get().get_stats("algo", #name).laps().back()))

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
    auto query = [&](const Bv& bv) -> bool
    {
        bool ret;
        SingletonProfiler::Pause("algo", "toosmall");

        ret = teacher.consider(bv);

        SingletonProfiler::Resume("algo", "toosmall");
        return ret;
    };
PROF_METHOD (
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
);
    return tmp;
}

const Feedback& Learner::result () const
{
    return fb;
}
//=================================================================================================
}

#undef PROF_METHOD
#undef PROF_ALGO
