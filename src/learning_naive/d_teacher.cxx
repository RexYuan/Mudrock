
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
            tmp[i] = varmap.at(static_cast<int>(aigvar));

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
}
D_Teacher::Feedback D_Teacher::consider (const vector<Face>& faces)
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
        return Feedback::TooSmall;
    // Hypt(X) => ~Bad(I,X)
    else if (!hold(hypt |= ~bad, m)) 
        return Feedback::TooBig;
    // Hypt(X), Trans(I,X,X') => Hypt(X')
    else if (!hold(hypt & trans |= hyptp, m))
        return judge(faces); // by heuristic
    // invariant found
    else
        return Feedback::Perfect;
}

D_Teacher::Feedback D_Teacher::judge (const vector<Face>& faces)
{
    return Feedback::Perfect;
}

ostream& operator << (ostream& out, const D_Teacher::Feedback& feedback)
{
    switch (feedback)
    {
        case D_Teacher::Feedback::Perfect:  out << "Perfect";  break;
        case D_Teacher::Feedback::TooBig:   out << "TooBig";   break;
        case D_Teacher::Feedback::TooSmall: out << "TooSmall"; break;
    }
    return out;
}
