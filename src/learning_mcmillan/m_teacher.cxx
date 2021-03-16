
#include "m_teacher.hxx"

M_Teacher::M_Teacher (const string& filename) :
m{Mana{}},
aig{Aig{filename}},
sw{m.newSw()}
{
    assert(aig);
    assert(aig.num_outputs() == 1);

    setup();
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
/*
 * prepare the first step of 0
 */
void M_Teacher::setup ()
{
    // set up variables over I,X,X'
    first_aig_varmap  = toBfmap(addAig(aig, m));
    second_aig_varmap = toBfmap(addAig(aig, m));

    // set up Init(I,X), Bad_{0}(I,X), Trans_{0}(I)
    auto f_init   = mk_init(aig, first_aig_varmap);
    auto f_bad    = mk_bad(aig, first_aig_varmap);
    auto f_trans  = mk_trans(aig, first_aig_varmap, second_aig_varmap);
    // set up Trans0(I,X,X')
    auto f_trans0 = f_trans;

    init   = v(addBf(f_init, m));
    bad    = v(addBf(f_bad, m));
    trans  = v(addBf(f_trans, m));
    trans0 = v(addBf(f_trans0, m));

    first_index_varmap  = mk_index_varmap(aig, first_aig_varmap);
    second_index_varmap = mk_index_varmap(aig, second_aig_varmap);

    last_aig_varmap   = second_aig_varmap;
    last_index_varmap = second_index_varmap;
}

/*
 * unroll one step to k+1 when the foresight is of length k
 */
void M_Teacher::unroll ()
{
    // set up variables over I,X_{k+1}
    auto tmp_aig_varmap = toBfmap(addAig(aig, m));

    // set up Bad_{k+1}(I, X_{k+1})
    auto f_bad   = mk_bad(aig, tmp_aig_varmap);
    // set up Trans_{k+1}(I, X_{k}, X_{k+1})
    auto f_trans = mk_trans(aig, last_aig_varmap, tmp_aig_varmap);

    // set up Bad(I, X_{0}, X_{1}, ..., X_{k+1})
    bad   = v(addBf(bad | f_bad, m));
    // set up Trans(I, X_{0}, X_{1}, ..., X_{k+1})
    trans = v(addBf(trans & f_trans, m));

    auto tmp_index_varmap = mk_index_varmap(aig, tmp_aig_varmap);
    last_index_varmap = tmp_index_varmap;
    last_aig_varmap = tmp_aig_varmap;
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
M_Types::Feedback M_Teacher::consider (const vector<Face>& faces)
{
    assert(first_index_varmap.size() > 0 && last_index_varmap.size() > 0);
    return (state = Perfect);
}

Bv M_Teacher::counterexample () const
{
    assert(state != Refuted && state != Perfect);
    assert(ce);
    return ce;
}

bool M_Teacher::degen ()
{
    if (sat(init & bad, m))
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

const M_Types::Feedback& M_Teacher::check_state () const
{
    return state;
}
