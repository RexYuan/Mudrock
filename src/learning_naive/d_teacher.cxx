
#include "d_teacher.hxx"

D_Teacher::D_Teacher (const string& filename) :
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
    // Init := X = 0
    inline Bf_ptr mk_init (const Aig& aig, const map<int,int>& curr_varmap)
    {
        Bf_ptr tmp = v(true);

        // set all latches to 0s
        for (const auto& [aigvar,aiglit] : aig.latches())
            tmp = tmp & ~toBf(aigvar);

        tmp = subst(tmp, curr_varmap);
        return tmp;
    }

    // Bad := aig output
    inline Bf_ptr mk_bad (const Aig& aig, const map<int,int>& curr_varmap)
    {
        Bf_ptr tmp = toBf(aig.outputs()[0]);
        tmp = subst(tmp, curr_varmap);
        return tmp;
    }

    // Trans := X' = X
    inline Bf_ptr mk_trans (const Aig& aig, const map<int,int>& curr_varmap,
                                            const map<int,int>& next_varmap)
    {
        Bf_ptr tmp = v(true);

        for (Bf_ptr x, xp; const auto& [aigvar,aiglit] : aig.latches())
        {
            xp = toBf(aigvar);
            x  = toBf(aiglit);

            xp = subst(xp, next_varmap);
            x  = subst(x,  curr_varmap);

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
void D_Teacher::setup ()
{
    // set up variables over I,X and I',X'
    auto curr_aig_varmap = toBfmap(addAig(aig, m));
    auto next_aig_varmap = toBfmap(addAig(aig, m));

    curr_index_varmap = mk_index_varmap(aig, curr_aig_varmap);
    next_index_varmap = mk_index_varmap(aig, next_aig_varmap);

    // set up Init(I,X), Bad(I,X), Trans(I,X,X')
    auto f_init  = mk_init (aig, curr_aig_varmap);
    auto f_bad   = mk_bad  (aig, curr_aig_varmap);
    auto f_trans = mk_trans(aig, curr_aig_varmap, next_aig_varmap);

    init  = v(addBf(f_init, m));
    bad   = v(addBf(f_bad, m));
    trans = v(addBf(f_trans, m));
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
            bit.setbit(m.val(v));
        return tmp;
    }
}
D_Types::Feedback D_Teacher::consider (const vector<Face>& faces)
{
    assert(curr_index_varmap.size() > 0 && next_index_varmap.size() > 0);

    Bf_ptr cdnf = mk_cdnf(faces);
    Bf_ptr curr_cdnf = subst(cdnf, curr_index_varmap),
           next_cdnf = subst(cdnf, next_index_varmap);
    m.releaseSw(sw);
    sw = m.newSw();
    hypt  = v(addBf(curr_cdnf, m, sw));
    hyptp = v(addBf(next_cdnf, m, sw));

    // Init(I,X) => Hypt(X)
    if (!hold(init |= hypt, m))
    {
        // X is positive counterexample
        ce = mk_ce(curr_index_varmap, m);
        return (state = TooSmall);
    }
    // Hypt(X) => ~Bad(I,X)
    else if (!hold(hypt |= ~bad, m))
    {
        // X is negative counterexample
        ce = mk_ce(curr_index_varmap, m);
        return (state = TooBig);
    }
    // Hypt(X), Trans(I,X,X') => Hypt(X')
    else if (!hold(hypt & trans |= hyptp, m))
    {
        // determine by heuristic
        return (state = judge(faces));
    }
    // invariant found
    else
    {
        return (state = Perfect);
    }
}

namespace
{
    // face1.basis | face2.basis ...
    inline Bf_ptr mk_char_dnf (const vector<Face>& faces)
    {
        vector<Bv> previous_negs;
        for (const Face& face : faces)
            previous_negs.push_back(face.basis());

        Bf_ptr disj = v(false);
        for (const Bv& neg : previous_negs)
            disj = disj | toBf(neg);

        return disj;
    }
}
/*
 * heuristic:
 *     if Hypt(X), Trans(I,X,X'), ~Hypt(X') is sat and X' is a previous negative counterexample,
 *     then X is a negative counterexample; else, X is a positive counterexample.
 *
 *     if X is a negative counterexample and Init(I,X) is sat, then a refutation is found.
 */
D_Types::Feedback D_Teacher::judge (const vector<Face>& faces)
{
    Bf_ptr dead_endsp = subst(mk_char_dnf(faces), next_index_varmap);
    if (sat(hypt & trans & ~hyptp & dead_endsp, m))
    {
        // X is negative counterexample
        ce = mk_ce(curr_index_varmap, m);

        // check refutation
        Bf_ptr is_ce = subst(toBf(ce), curr_index_varmap);
        if (sat(is_ce & init, m))
            return Refuted;

        return TooBig;
    }
    else
    {
        // X' is positive counterexample
        ce = mk_ce(next_index_varmap, m);
        return TooSmall;
    }
}

Bv D_Teacher::counterexample () const
{
    assert(state != Refuted && state != Perfect);
    return ce;
}
