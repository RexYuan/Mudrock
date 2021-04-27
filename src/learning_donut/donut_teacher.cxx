
#include "donut_teacher.hxx"

#define SAT_NAME_ID_TRAIL(name, id)                                           \
    (string(name) == ""s ? string(id) : string(name) + " "s + string(id))

#define PROF_SAT_AS(name, code...)                                            \
    ({                                                                        \
        PROF_SCOPE_WITH_ID("sat", SAT_NAME_ID_TRAIL(name, "sat"s));           \
        code;                                                                 \
    })

#define PROF_SAT(code...)                                                     \
    PROF_SAT_AS(""s, code)

namespace Donut
{
//=================================================================================================
Teacher::Teacher (const string& filename) :
m{Mana{}},
aig{Aig{filename}},
cumu_sw{m.newSw()}, tent_sw{m.newSw()}
{
    assert(aig);
    assert(aig.num_outputs() == 1);
}

void Teacher::renewMana ()
{
PROF_SCOPE();
log(3, "Teacher", "Began renewing");
    // renew mana
    m.~Mana();
    new (&m) Mana{};

    cumu_sw = m.newSw();
    tent_sw = m.newSw();

    // restore constraints
    setup();
    size_t tmp = unroll_depth;
    unroll(unroll_depth);
    unroll_depth = tmp; // restore depth from over-incrementing

    // restore frontiers
    last_frnt  = v(addBf(f_last_frnt_cache, m));
    last_frntp = v(addBf(f_last_frntp_cache, m));
    frnt       = v(addBf(f_frnt_cache, m));
    frntp      = v(addBf(f_frntp_cache, m));
log(3, "Teacher", "Ended renewing");
}

namespace
{
    // Init := X_{0} = 0
    inline Bf_ptr mk_init (const Aig& aig, const map<int,int>& first_varmap)
    {
        Bf_ptr tmp = v(true);

        // set all latches to 0s
        for (const auto& [aigvar,aiglit] : aig.latches())
            tmp = tmp & ~toBf(aigvar);

        tmp = subst(tmp, first_varmap);
        return tmp;
    }

    // Bad_{k} := output(X_{k})
    inline Bf_ptr mk_bad (const Aig& aig, const map<int,int>& k_varmap)
    {
        Bf_ptr tmp = toBf(aig.outputs()[0]);
        tmp = subst(tmp, k_varmap);
        return tmp;
    }

    // Trans_{k} := X_{k} = X_{k-1}; succ(`k`)=`kp`
    inline Bf_ptr mk_trans (const Aig& aig, const map<int,int>& k_varmap,
                                            const map<int,int>& kp_varmap)
    {
        Bf_ptr tmp = v(true);

        for (Bf_ptr x, xp; const auto& [aigvar,aiglit] : aig.latches())
        {
            xp = toBf(aigvar);
            x  = toBf(aiglit);

            xp = subst(xp, kp_varmap);
            x  = subst(x,  k_varmap);

            tmp = tmp & (xp == x);
        }

        return tmp;
    }

    // from range of AigVar=>Var to {0...latches.size()}=>Var
    inline map<int,int> mk_index_varmap (const Aig& aig, const map<int,int>& varmap)
    {
        map<int,int> tmp;

        // zip range{0, ..., num_latches} to Vars corresponding to latches AigVars in varmap
        for (size_t i=0; const auto& [aigvar,aiglit] : aig.latches())
            tmp[i++] = varmap.at(static_cast<int>(aigvar));

        return tmp;
    }
}
//=================================================================================================
// Higher order commands for context
//
void Teacher::setup ()
{
PROF_SCOPE();
    // set up variables over X,X'
    first_aig_varmap  = toBfmap(addAig(aig, m));
    second_aig_varmap = toBfmap(addAig(aig, m));

    // set up Init(X), Bad(X'), Trans(X,X'), Trans()
    auto f_init     = mk_init(aig, first_aig_varmap);
    auto f_bad      = mk_bad(aig, second_aig_varmap);
    auto f_trans_hd = mk_trans(aig, first_aig_varmap, second_aig_varmap);
    auto f_trans_tl = v(true);

    init     = v(addBf(f_init, m));
    bad      = v(addBf(f_bad, m));
    trans_hd = v(addBf(f_trans_hd, m));
    trans_tl = v(addBf(f_trans_tl, m));

    first_index_varmap  = mk_index_varmap(aig, first_aig_varmap);
    second_index_varmap = mk_index_varmap(aig, second_aig_varmap);

    last_aig_varmap = second_aig_varmap;
    last_index_varmap = second_index_varmap;
}

// unroll bad by `n` step
void Teacher::unroll (size_t n)
{
PROF_SCOPE();
    for (size_t i=0; i<n; i++)
    {
        // set up variables over X''
        auto tmp_aig_varmap = toBfmap(addAig(aig, m));

        // set up Bad(X''), Trans(X',X'')
        auto f_bad      = mk_bad(aig, tmp_aig_varmap);
        auto f_trans_tl = mk_trans(aig, last_aig_varmap, tmp_aig_varmap);

        bad      = v(addBf(bad | f_bad, m));
        trans_tl = v(addBf(trans_tl & f_trans_tl, m));

        last_aig_varmap = tmp_aig_varmap;
        last_index_varmap = mk_index_varmap(aig, tmp_aig_varmap);

        unroll_depth++;
    }
}

// if init meets bad
bool Teacher::degen ()
{
PROF_SCOPE();
    bool ret;
    // I(X), T(X,X'), T(X',X'',...), B(X',X'',...)
    if (PROF_SAT(sat(init & trans_hd & trans_tl & bad, m)))
    {
        state = Refuted;
        ret = true;
    }
    else
    {
        state = Unknown;
        ret = false;
    }
    return ret;
}

// if frontier image doesn't meet bad
bool Teacher::advanceable ()
{
PROF_SCOPE();
    bool ret;
    // last H(X), T(X,X'), T(X',X'',...), B(X',X'',...)
    if (PROF_SAT(sat(last_frnt & trans_hd & trans_tl & bad, m)))
        ret = false;
    else
        ret = true;
    return ret;
}

// reset frontier back to init
void Teacher::restart ()
{
PROF_SCOPE();
    m.releaseSw(cumu_sw);
    m.releaseSw(tent_sw);
    cumu_sw = m.newSw();
    tent_sw = m.newSw();

    f_last_frnt_cache  = last_frnt  = init;
    f_last_frntp_cache = last_frntp = init;
    f_frnt_cache       = frnt       = v(false);
    f_frntp_cache      = frntp      = v(false);
}

// if current frontier > last frontier
bool Teacher::progressed ()
{
PROF_SCOPE();
    bool ret;
    // H(X) <= last H(X) and H is non-empty means no progress
    if (PROF_SAT(hold(frnt |= last_frnt, m) && sat(frnt, m)))
        ret = false;
    else
        ret = true;
    return ret;
}

// advance frontier
void Teacher::advance ()
{
PROF_SCOPE();
    // cumulate progress
    frnt  = v(addBf(f_frnt_cache,  m, cumu_sw));
    frntp = v(addBf(f_frntp_cache, m, cumu_sw));

    // induction by
    // last H(X)_0 = init
    // last H(X)_k = last H(X)_k-1 | H(X)
    last_frnt  = v(addBf(frnt  | last_frnt,  m, cumu_sw));
    last_frntp = v(addBf(frntp | last_frntp, m, cumu_sw));

    f_last_frnt_cache  = f_frnt_cache  | f_last_frnt_cache;
    f_last_frntp_cache = f_frntp_cache | f_last_frntp_cache;
}

namespace
{
    inline Bf_ptr mk_cdnf (const vector<Face>& faces)
    {
        Bf_ptr tmp = v(true);
        for (const auto& face : faces)
            tmp = tmp & toBf(face);
        return tmp;
    }

    // extract the valuation Bv in the range of `index_varmap`
    inline Bv mk_ce (const map<int,int>& index_varmap, const Mana& m)
    {
        Bv tmp{index_varmap.size()};
        auto bit = tmp.begin();
        for (const auto& [i,v] : index_varmap)
        {
            bit.setbit(m.val(v));
            bit++;
        }
        return tmp;
    }
}
//=================================================================================================
// Query commands for learner
//
bool Teacher::membership (Bv bv)
{
PROF_SCOPE();
    bool ret;
    // accept all X' that is not in T(X',X'',...), B(X',X'',...)
    if (PROF_SAT(evaluate(last_frnt & trans_hd, m, bv, second_index_varmap)))
        ret = true;
    else
        ret = false;
    return ret;
}

// if frontier image < `faces` < bad
Feedback Teacher::equivalence (const vector<Face>& faces)
{
PROF_SCOPE();
    Feedback ret;
    assert(first_index_varmap.size() > 0 && last_index_varmap.size() > 0);

    m.releaseSw(tent_sw);
    tent_sw = m.newSw();

    Bf_ptr cdnf = mk_cdnf(faces);
    // H(X), H(X')
    Bf_ptr first_cdnf  = subst(cdnf, first_index_varmap),
           second_cdnf = subst(cdnf, second_index_varmap);
    frnt  = v(addBf(f_frnt_cache  = first_cdnf,  m, tent_sw));
    frntp = v(addBf(f_frntp_cache = second_cdnf, m, tent_sw));

    // progress criterion (forward image over-approximation)
    //=========================================================================
    // last H(X), T(X,X') => H(X')
    if (PROF_SAT_AS("progress"s,
        !hold(last_frnt & trans_hd |= frntp, m)))
    {
        // X' is positive counterexample
        ce = mk_ce(second_index_varmap, m);
        ret = (state = TooSmall);
    }
    // soundness criterion with foresight (unrolled ~bad under-approximation)
    //=========================================================================
    // H(X'), T(X',X'',...) => ~B(X',X'',...)
    else if (PROF_SAT_AS("soundness"s,
             !hold(frntp & trans_tl |= ~bad, m)))
    {
        // X' is negative counterexample
        ce = mk_ce(second_index_varmap, m);
        ret = (state = TooBig);
    }
    // done
    else
    {
        ret = (state = Perfect);
    }
    return ret;
}

Bv Teacher::counterexample () const
{
PROF_SCOPE();
    assert(state != Refuted && state != Perfect && state != Unknown);
    assert(ce);
    return ce;
}

const Feedback& Teacher::check_state () const
{
PROF_SCOPE();
    return state;
}
//=================================================================================================
}

#undef SAT_NAME_ID_TRAIL
#undef PROF_SAT_AS
#undef PROF_SAT
