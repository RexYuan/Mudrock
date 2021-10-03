
#include "donut_learner.hxx"

#define PROF_ALGO_SCOPE_AS(name)                                              \
        PROF_SCOPE_WITH_ID("algo", name)

#define PROF_ALGO_PAUSE(name)                                                 \
    SingletonProfiler::Pause(ID_NAME(__FILE__, "algo"), ID_NAME(__func__, name));

#define PROF_ALGO_RESUME(name)                                                \
    SingletonProfiler::Resume(ID_NAME(__FILE__, "algo"), ID_NAME(__func__, name));

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
    ce = nullptr;
    hypts.clear();
}

void Learner::learn ()
{
PROF_SCOPE();
    auto query = [&](const vector<Face>& faces) -> Feedback
    {
        Feedback ret;

        ret = teacher.equivalence(hypts);

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
        if (terminate_requested)
        {
            log(3, "Top", "SIGTERM received. Breaking out of learn loop");
            break;
        }
        switch (fb)
        {
            // needs g++-11
            case Perfect: goto ret; // we're done
            case Refuted: [[fallthrough]];
            case Unknown: assert(false); // shouldn't happen
            case TooBig:
            {
            PROF_ALGO_SCOPE_AS("toobig"s);
                ce = teacher.counterexample(); // negative ce
                assert(ce);
                hypts.push_back(Face{ce}); // add a new councillor
                break;
            }
            case TooSmall:
            {
            PROF_ALGO_SCOPE_AS("toosmall"s);
                ce = teacher.counterexample(); // positive ce
                for (auto& hypt : hypts)
                {
                    if (!hypt(ce))
                    {
            PROF_ALGO_PAUSE("toosmall"s);
                        ce = minimize(ce, hypt);
            PROF_ALGO_RESUME("toosmall"s);
                        hypt.push(ce); // augment councillor
                    }
                }
                break;
            }
        }
        fb = query(hypts);
    }
ret:
    return;
}

Bv_ptr Learner::minimize (const Bv_ptr ce, const Face& f)
{
PROF_SCOPE();
    auto query = [&](const Bv_ptr bv) -> bool
    {
        bool ret;

        ret = teacher.membership(bv);

        return ret;
    };
    Bv_ptr tmp = mkBv(ce);
    for (size_t i=0; i<ce->len(); i++)
    {
        if (terminate_requested)
        {
            log(3, "Top", "SIGTERM received. Breaking out of minimize loop");
            break;
        }
        if (tmp->operator[](i) != f.basis()->operator[](i))
        {
            tmp->flipper(i);
            if (query(tmp))
                i = 0;
            else
                tmp->flipper(i);
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

#undef PROF_ALGO_SCOPE_AS
#undef PROF_ALGO_PAUSE
#undef PROF_ALGO_RESUME
