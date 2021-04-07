
#include "donut_teacher.hxx"

namespace Donut
{
//=================================================================================================
Teacher::Teacher (const string& filename, TeacherProfiler& p) :
prof{p},
m{Mana{}},
aig{Aig{filename}},
sw{m.newSw()}
{
    assert(aig);
    assert(aig.num_outputs() == 1);
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
    start(prof.teacher_total, prof.setup_time);

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

    stop(prof.teacher_total, prof.setup_time);

    // prep frontiers
    restart();
}

// unroll bad by `n` step
void Teacher::unroll (size_t n)
{
    start(prof.teacher_total, prof.unroll_time);

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
    }

    stop(prof.teacher_total, prof.unroll_time);
}

// if init meets bad
bool Teacher::degen ()
{
    bool ret, tmp;
    start(prof.teacher_total, prof.degen_time);

    // I(X), T(X,X'), T(X',X'',...), B(X',X'',...)
    if (start(prof.sat_total, prof.degen_sat),
        tmp = sat(init & trans_hd & trans_tl & bad, m),
        stop(prof.sat_total, prof.degen_sat), tmp)
    {
        state = Refuted;
        ret = true;
    }
    else
    {
        state = Unknown;
        ret = false;
    }

    stop(prof.teacher_total, prof.degen_time);
    return ret;
}

// if frontier image doesn't meet bad
bool Teacher::advanceable ()
{
    bool ret, tmp;
    start(prof.teacher_total, prof.advanceable_time);

    // last H(X), T(X,X'), T(X',X'',...), B(X',X'',...)
    if (start(prof.sat_total, prof.advanceable_sat),
        tmp = sat(last_frnt & trans_hd & trans_tl & bad, m),
        stop(prof.sat_total, prof.advanceable_sat), tmp)
        ret = false;
    else
        ret = true;

    stop(prof.teacher_total, prof.advanceable_time);
    return ret;
}

// reset frontier back to init
void Teacher::restart ()
{
    start(prof.teacher_total, prof.restart_time);

    m.releaseSw(sw);
    sw = m.newSw();

    last_frnt  = init;
    last_frntp = init;
    frnt  = v(false);
    frntp = v(false);

    stop(prof.teacher_total, prof.restart_time);
}

// if current frontier > last frontier
bool Teacher::progressed ()
{
    bool ret, tmp;
    start(prof.teacher_total, prof.progressed_time);

    // H(X) <= last H(X) and H is non-empty means no progress
    if (start(prof.sat_total, prof.progressed_sat),
        tmp = hold(frnt |= last_frnt, m) && sat(frnt, m),
        stop(prof.sat_total, prof.progressed_sat), tmp)
        ret = false;
    else
        ret = true;

    stop(prof.teacher_total, prof.progressed_time);
    return ret;
}

// advance frontier
void Teacher::advance ()
{
    start(prof.teacher_total, prof.advance_time);

    // induction by
    // last H(X)_0 = init
    // last H(X)_k = last H(X)_k-1 | H(X)
    last_frnt  = v(addBf(frnt | last_frnt, m));
    last_frntp = v(addBf(frntp | last_frntp, m));

    stop(prof.teacher_total, prof.advance_time);
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
bool Teacher::consider (Bv bv)
{
    bool ret, tmp;
    start(prof.teacher_total, prof.membership_time);

    vector<Var> range;
    Bf_ptr bf = toBf(bv);
    bf = subst(bf, second_index_varmap);
    // accept all X' that is not in T(X',X'',...), B(X',X'',...)
    if (start(prof.sat_total, prof.membership_sat),
        tmp = sat(bf & trans_tl & bad, m),
        stop(prof.sat_total, prof.membership_sat), tmp)
        ret = false;
    else
        ret = true;

    stop(prof.teacher_total, prof.membership_time);
    return ret;
}

// if frontier image < `faces` < bad
Feedback Teacher::consider (const vector<Face>& faces)
{
    Feedback ret; bool tmp;
    start(prof.teacher_total, prof.equivalence_time);

    assert(first_index_varmap.size() > 0 && last_index_varmap.size() > 0);

    Bf_ptr cdnf = mk_cdnf(faces);
    // H(X), H(X')
    Bf_ptr first_cdnf  = subst(cdnf, first_index_varmap),
           second_cdnf = subst(cdnf, second_index_varmap);
    frnt  = v(addBf(first_cdnf, m, sw));
    frntp = v(addBf(second_cdnf, m, sw));

    // progress criterion (forward image over-approximation)
    //=========================================================================
    // last H(X), T(X,X') => H(X')
    if (start(prof.sat_total, prof.equivalence_progress_sat),
        tmp = !hold(last_frnt & trans_hd |= frntp, m),
        stop(prof.sat_total, prof.equivalence_progress_sat), tmp)
    {
        // X' is positive counterexample
        ce = mk_ce(second_index_varmap, m);
        ret = (state = TooSmall);
    }
    // soundness criterion with foresight (unrolled ~bad under-approximation)
    //=========================================================================
    // H(X'), T(X',X'',...) => ~B(X',X'',...)
    else if (start(prof.sat_total, prof.equivalence_soundness_sat),
             tmp = !hold(frntp & trans_tl |= ~bad, m),
             stop(prof.sat_total, prof.equivalence_soundness_sat), tmp)
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

    stop(prof.teacher_total, prof.equivalence_time);
    return ret;
}

Bv Teacher::counterexample () const
{
    assert(state != Refuted && state != Perfect && state != Unknown);
    assert(ce);
    return ce;
}

const Feedback& Teacher::check_state () const
{
    return state;
}
//=================================================================================================
}
