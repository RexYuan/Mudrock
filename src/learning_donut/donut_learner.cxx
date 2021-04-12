
#include "donut_learner.hxx"

// usage:
//   PROF_METHOD(method_name, code);
#define PROF_METHOD(method_name, code...) \
    log(1, "Learner", "Began " #method_name); \
    start(prof.method_name); \
    code \
    stop(prof.method_name); \
    log(1, "Learner", "Ended " #method_name ", time spent: ", \
        to_string(prof.method_name.laps().back()))

// usage:
//   PROF_ALGO(algo_name, code);
#define PROF_ALGO(algo_name, code...) \
    log(2, "Learner", "Began " #algo_name); \
    start(prof.algo_total, prof.algo_name); \
    code \
    stop(prof.algo_total, prof.algo_name); \
    log(2, "Learner", "Ended " #algo_name ", time spent: ", \
        to_string(prof.algo_name.laps().back()))

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
PROF_ALGO (toobig,
                ce = teacher.counterexample(); // negative ce
                assert(ce);
                hypts.push_back(Face{ce}); // add a new councillor
);
                break;
            }
            case TooSmall:
            {
PROF_METHOD (minimize,
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
);
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
        pause(prof.algo_total, prof.toosmall);

        ret = teacher.consider(bv);

        resume(prof.algo_total, prof.toosmall);
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

#undef PROF_METHOD
#undef PROF_ALGO
