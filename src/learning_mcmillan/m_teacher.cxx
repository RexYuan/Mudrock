
#include "m_teacher.hxx"

M_Teacher::M_Teacher (const string& filename) :
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
void M_Teacher::setup ()
{
    // set up variables over X,X'
    first_aig_varmap  = toBfmap(addAig(aig, m));
    second_aig_varmap = toBfmap(addAig(aig, m));

    // set up Init(X), Bad(X,X'), Trans(X,X'), Trans()
    auto f_init     = mk_init(aig, first_aig_varmap);
    auto f_bad      = mk_bad(aig, first_aig_varmap) | mk_bad(aig, second_aig_varmap);
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

    // prep frontiers
    restart();
}

// unroll bad by `n` step
void M_Teacher::unroll (size_t n)
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

// if init meets bad
bool M_Teacher::degen ()
{
    // I(X), T(X,X'), T(X',X'',...), B(X,X',...)
    if (sat(init & trans_hd & trans_tl & bad, m))
    {
        state = Refuted;
        return true;
    }
    else
    {
        state = Unknown;
        return false;
    }
}

// if frontier image doesn't meet bad
bool M_Teacher::advanceable ()
{
    // H(X), T(X,X'), T(X',X'',...), B(X,X',...)
    if (sat(frnt & trans_hd & trans_tl & bad, m))
        return false;
    return true;
}

// reset frontier back to init
void M_Teacher::restart ()
{
    last_frnt  = init;
    last_frntp = init;
    frnt  = v(true);
    frntp = v(true);
}

// if current frontier > last frontier
bool M_Teacher::progressed ()
{
    // H(X) => last H(X)
    if (hold(frnt |= last_frnt, m))
        return true;
    return false;
}

// advance frontier
void M_Teacher::advance ()
{
    last_frnt  = frnt;
    last_frntp = frntp;
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
// if frontier image < `faces` < bad
M_Types::Feedback M_Teacher::consider (const vector<Face>& faces)
{
    assert(first_index_varmap.size() > 0 && last_index_varmap.size() > 0);

    Bf_ptr cdnf = mk_cdnf(faces);
    // H(X), H(X')
    Bf_ptr first_cdnf  = subst(cdnf, first_index_varmap),
           second_cdnf = subst(cdnf, second_index_varmap);
    m.releaseSw(sw);
    sw = m.newSw();
    frnt  = v(addBf(first_cdnf, m, sw));
    frntp = v(addBf(second_cdnf, m, sw));

    // induction criterion (a fortiori, init correctness)
    //=========================================================================
    // last H(X) => H(X)
    if (!hold(last_frnt |= frnt, m))
    {
        // X is positive counterexample
        ce = mk_ce(first_index_varmap, m);
        return (state = TooSmall);
    }
    // progress criterion (forward image over-approximation)
    //=========================================================================
    // last H(X), T(X,X') => H(X')
    else if (!hold(last_frnt & trans_hd |= frnt, m))
    {
        // X' is positive counterexample
        ce = mk_ce(second_index_varmap, m);
        return (state = TooSmall);
    }
    // soundness criterion with foresight (unrolled bad under-approximation)
    //=========================================================================
    // H(X), T(X,X'), T(X',X'',...) => ~B(X,X',X'',...)
    else if (!hold(frnt & trans_hd & trans_tl |= ~bad, m))
    {
        // X is negative counterexample
        ce = mk_ce(first_index_varmap, m);
        return (state = TooBig);
    }
    // done
    else
    {
        return (state = Perfect);
    }
}

Bv M_Teacher::counterexample () const
{
    assert(state != Refuted && state != Perfect && state != Unknown);
    assert(ce);
    return ce;
}

const M_Types::Feedback& M_Teacher::check_state () const
{
    return state;
}
