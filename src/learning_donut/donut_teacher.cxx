
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

#define COUNT_CLAUSE(thing)                                                   \
    log(4, "Count", string(#thing) + " clause size = "s +                     \
                    to_string(dryCountClause(thing)))

#ifdef COUNTING
#define addBf(bf, args...)                                                    \
    ({                                                                        \
        log(4, "Count", "Counting addBf");                                    \
        COUNT_CLAUSE(bf);                                                     \
        addBf(bf, args);                                                      \
    })

#define addAig(aig, args...)                                                  \
    ({                                                                        \
        log(4, "Count", "Counting addAig");                                   \
        COUNT_CLAUSE(aig);                                                    \
        addAig(aig, args);                                                    \
    })

#define extendAig(varmap, aig, args...)                                       \
    ({                                                                        \
        log(4, "Count", "Counting extendAig");                                \
        COUNT_CLAUSE(aig);                                                    \
        extendAig(varmap, aig, args);                                         \
    })
#endif

namespace Donut
{
//=================================================================================================
Teacher::Teacher (const string& filename) :
aig{Aig{filename}},
mm{aig}, fm{aig}, cm{aig}
{
    assert(aig);
    assert(aig.num_outputs() == 1);
}

//=================================================================================================
// Higher order commands for context
//
void Teacher::setup ()
{
PROF_SCOPE();
    mm.setup();
    fm.setup();
    cm.setup();
}

// unroll bad by `n` step
void Teacher::unroll (size_t n)
{
PROF_SCOPE();
    cm.unroll(n);
}

// reset frontier back to init
void Teacher::restart ()
{
PROF_SCOPE();
    mm.restart();
    fm.restart();
    cm.restart();
}

// advance frontier
void Teacher::advance ()
{
PROF_SCOPE();
    mm.advance(cdnf_cache);
    fm.advance();
    cm.advance();
}

// if init reaches bad
bool Teacher::reachbad ()
{
PROF_SCOPE();
    bool ret;
    ret = cm.reachbad();
    if (ret)
        state = Refuted;
    else
        state = Unknown;
    return ret;
}

// if frontier image doesn't meet bad
bool Teacher::meetbad ()
{
PROF_SCOPE();
    return cm.meetbad();
}

// if current frontier <= last frontier
bool Teacher::fixedpoint ()
{
PROF_SCOPE();
    return fm.fixedpoint();
}

namespace
{
    inline Bf_ptr mk_cdnf (const vector<Face>& faces)
    {
        Bf_ptr tmp = conj();
        tmp->reserve(faces.size());
        for (const auto& face : faces)
            tmp += toBf(face);
        return tmp;
    }

    // extract the valuation Bv in the range of `state_varmap`
    inline Bv_ptr mk_ce (const vector<Var>& state_varmap, const Mana& m)
    {
        Bv_ptr tmp = mkBv(state_varmap.size());
        auto bit = tmp->begin();
        for (const Var v : state_varmap)
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
bool Teacher::membership (const Bv_ptr bv)
{
PROF_SCOPE();
    return mm.membership(bv);
}

// if frontier image < `faces` < bad
Feedback Teacher::equivalence (const vector<Face>& faces)
{
PROF_SCOPE();
    Feedback ret;

    Bf_ptr cdnf = cdnf_cache = mk_cdnf(faces);
    // progress criterion (forward image over-approximation)
    //=========================================================================
    // last H(X), T(X,X') => H(X')
    if (fm.progress(cdnf))
    {
        // X' is positive counterexample
        ce = fm.get_ce();
        ret = (state = TooSmall);
    }
    // soundness criterion with foresight (unrolled ~bad under-approximation)
    //=========================================================================
    // H(X'), T(X',X'',...) => ~B(X',X'',...)
    // DEBUG: this takes too much time
    else if (cm.soundness(cdnf))
    {
        // X' is negative counterexample
        ce = cm.get_ce();
        ret = (state = TooBig);
    }
    // done
    else
    {
        ret = (state = Perfect);
    }
    return ret;
}

Bv_ptr Teacher::counterexample () const
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

vector<Bv_ptr> Teacher::witness () const
{
    vector<Bv_ptr> stimulus;
    for (const auto& aig_varmap : cm.aig_varmaps_cache)
    {
        Bv_ptr tmp = mkBv(aig.num_inputs());
        auto bit = tmp->begin();
        for (const auto& aigvar : aig.inputs())
        {
            bit.setbit(cm.m.val(aig_varmap.at(aigvar)));
            bit++;
        }
        stimulus.push_back(tmp);
    }
    return stimulus;
}

//=================================================================================================
// Query fromulae builders and helpers
namespace
{
    // X = 0 with state vars
    inline Bf_ptr mk_state_init (const Aig& aig)
    {
        Bf_ptr tmp = conj();
        tmp->reserve(aig.latches().size());

        // set all latches to 0s
        for (size_t i = 0; i < aig.latches().size(); i++)
            tmp += ~toBf(i);

        return tmp;
    }

    // Init := X_{0} = 0
    inline Bf_ptr mk_init (const Aig& aig, const vector<Var>& first_varmap)
    {
        Bf_ptr tmp = conj();
        tmp->reserve(aig.latches().size());

        // set all latches to 0s
        for (const auto& [aigvar,aiglit] : aig.latches())
            tmp += ~toBf(aigvar);

        tmp = subst(tmp, first_varmap);
        return tmp;
    }

    // Bad_{k} := output(X_{k})
    inline Bf_ptr mk_bad (const Aig& aig, const vector<Var>& k_varmap)
    {
        Bf_ptr tmp = toBf(aig.outputs()[0]);
        tmp = subst(tmp, k_varmap);
        return tmp;
    }

    // Trans_{k} := X_{k} = X_{k-1}; succ(`k`)=`kp`
    inline Bf_ptr mk_trans (const Aig& aig, const vector<Var>& k_varmap,
                                            const vector<Var>& kp_varmap)
    {
        Bf_ptr tmp = conj();
        tmp->reserve(aig.latches().size());

        for (Bf_ptr x, xp; const auto& [aigvar,aiglit] : aig.latches())
        {
            xp = toBf(aigvar);
            x  = toBf(aiglit);

            xp = subst(xp, kp_varmap);
            x  = subst(x,  k_varmap);

            tmp += (xp == x);
        }

        return tmp;
    }

    // from range of AigVar=>Var to {0...latches.size()}=>Var
    inline vector<Var> mk_state_varmap (const Aig& aig, const vector<Var>& varmap)
    {
        // NOTE: this is a trap because brace init calls the init list ctor
        vector<Var> tmp(aig.num_latches(), var_Undef);

        // zip range{0, ..., num_latches} to Vars corresponding to latches AigVars in varmap
        for (size_t i=0; const auto& [aigvar,aiglit] : aig.latches())
            tmp[i++] = varmap[aigvar];

        return tmp;
    }
}

bool Teacher::degen ()
{
PROF_SCOPE();
    Mana degen_m;

    // set up variables over X
    auto degen_first_aig_varmap  = addAig(aig, degen_m);

    // set up Init(X), Bad(X')
    auto f_init     = mk_init(aig, degen_first_aig_varmap);
    auto f_bad      = mk_bad(aig,  degen_first_aig_varmap);
    auto degen_init = v(addBf(f_init, degen_m));
    auto degen_bad  = v(addBf(f_bad,  degen_m));

    return PROF_SAT(sat(degen_init & degen_bad, degen_m));
}

// Member Manager
//
MemberMana::MemberMana (const Aig& aig_) :
aig{aig_}
{}

void MemberMana::setup ()
{
    // set up variables over X,X' in solver
    auto first_aig_varmap  = addAig(aig, m);
    auto second_aig_varmap = addAig(aig, m);
    first_state_varmap  = mk_state_varmap(aig, first_aig_varmap);
    second_state_varmap = mk_state_varmap(aig, second_aig_varmap);

    // set up Init(X), Trans(X,X') in solver var
    f_init  = mk_init(aig, first_aig_varmap);
    f_trans = mk_trans(aig, first_aig_varmap, second_aig_varmap);
    fixBf(f_trans, m);
}

void MemberMana::restart ()
{
    renew();
    setup();

    fixBf(f_init, m);

    f_cumu_hypt = disj();
    f_cumu_hypt += mk_state_init(aig); // in state vars
}

void MemberMana::advance (Bf_ptr cdnf)
{
    renew();
    setup();

    f_cumu_hypt += cdnf;

    Bf_ptr first_cdnf = subst(f_cumu_hypt, first_state_varmap);

    fixBf(first_cdnf, m);
}

void MemberMana::renew ()
{
    m.~Mana();
    new (&m) Mana{};
}

bool MemberMana::membership(const Bv_ptr bv)
{
    bool ret;
    // accept X' when last H(X), T(X,X') is sat
    if (PROF_SAT(evaluate(m, bv, second_state_varmap)))
        ret = true;
    else
        ret = false;
    return ret;
}

// Full Transition Manager
//
FullMana::FullMana (const Aig& aig_) :
aig{aig_},
tent_sw{m.newSw()}, cumu_sw{m.newSw()}
{}

void FullMana::setup ()
{
    // set up variables over X,X' in solver
    auto first_aig_varmap  = addAig(aig, m);
    auto second_aig_varmap = extendAig(first_aig_varmap, aig, m);
    first_state_varmap  = mk_state_varmap(aig, first_aig_varmap);
    second_state_varmap = mk_state_varmap(aig, second_aig_varmap);

    // set up Init(X), Trans(X,X') in solver var
    f_init  = mk_init(aig, first_aig_varmap);
}

void FullMana::restart ()
{
    renew();
    setup();

    // fixBf(f_init, m);
    cumu_hypt = v(addBf(f_init, m));
}

void FullMana::advance ()
{
    cumu_hypt = v(addBf(frnt | cumu_hypt,  m, cumu_sw));
}

void FullMana::renew ()
{
    m.~Mana();
    new (&m) Mana{};
    tent_sw = m.newSw();
    cumu_sw = m.newSw();
}

bool FullMana::progress (Bf_ptr cdnf)
{
    m.releaseSw(tent_sw);
    tent_sw = m.newSw();

    auto first_cdnf  = subst(cdnf, first_state_varmap);
    auto second_cdnf = subst(cdnf, second_state_varmap);
    frnt  = v(addBf(first_cdnf,  m, tent_sw));
    frntp = v(addBf(second_cdnf, m, tent_sw));

    bool ret;
    // last H(X), T(X,X') => H(X') hold
    // last H(X), T(X,X'), ~H(X') sat
    if (PROF_SAT(sat(cumu_hypt & ~frntp, m)))
    {
        ret = true;
        ce = mk_ce(second_state_varmap, m);
    }
    else
    {
        ret = false;
        frnt = v(addBf(first_cdnf,  m, cumu_sw));
    }
    return ret;
}

Bv_ptr FullMana::get_ce ()
{
    assert(ce);
    return ce;
}

bool FullMana::fixedpoint ()
{
    bool ret;
    // H(X) => last H(X) and H is non-empty means no progress
    // H(X) => last H(X) hold
    // H(X), ~last H(X) unsat
    if (PROF_SAT(!sat(frnt & ~cumu_hypt, m) && sat(frnt, m)))
        ret = true;
    else
        ret = false;
    return ret;
}

// Cone Of Influence Transition Manager
//
CoiMana::CoiMana (const Aig& aig_) :
aig{aig_},
tent_sw{m.newSw()}, cumu_sw{m.newSw()}
{
    cone = findCOIof(aig, aig.outputs()[0].var);
    cout << "SAVED " << (aig.latches().size() - cone.size()) << " LATCHES" << endl;
}

void CoiMana::setup ()
{
    // set up variables over X,X' in solver
    first_aig_varmap  = addAig(aig, m);
    second_aig_varmap = addAig(aig, m);
    last_aig_varmap   = second_aig_varmap;
    first_state_varmap  = mk_state_varmap(aig, first_aig_varmap);
    second_state_varmap = mk_state_varmap(aig, second_aig_varmap);
    last_state_varmap   = second_state_varmap;

    // set up Init(X), Bad(X') in solver var
    auto f_init = mk_init(aig, first_aig_varmap);
    auto f_badp = mk_bad(aig, second_aig_varmap);

    init = v(addBf(f_init, m));
    badp = v(addBf(f_badp, m));

    aig_varmaps_cache.push_back(first_aig_varmap);
    aig_varmaps_cache.push_back(second_aig_varmap);

    // the transition copy tail requires the inited head to work
    auto f_trans_hd = mk_trans(aig, first_aig_varmap, second_aig_varmap);
    auto f_trans_tl = v(true);
    trans_hd = v(addBf(f_trans_hd, m));
    trans_tl = v(addBf(f_trans_tl, m));
}

void CoiMana::restart ()
{
    m.releaseSw(cumu_sw);
    cumu_sw = m.newSw();

    cumu_hypt = init;
}

void CoiMana::advance ()
{
    cumu_hypt = v(addBf(frnt | cumu_hypt,  m, cumu_sw));
}

void CoiMana::unroll (size_t n)
{
    for (size_t i=0; i<n; i++)
    {
        // set up X''
        auto tmp_aig_varmap = extendAigSel(cone, last_aig_varmap, aig, m);

        // set up Bad(X'')
        auto f_bad = mk_bad(aig, tmp_aig_varmap);

        badp = v(addBf(badp | f_bad, m));

        last_aig_varmap = tmp_aig_varmap;
        last_state_varmap = mk_state_varmap(aig, tmp_aig_varmap);

        aig_varmaps_cache.push_back(last_aig_varmap);

        unroll_depth++;
    }
}

bool CoiMana::reachbad ()
{
    bool ret;
    // I(X), T(X,X'), T(X',X'',...), B(X',X'',...)
    if (PROF_SAT(sat(init & trans_hd & badp, m)))
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    return ret;
}

bool CoiMana::meetbad ()
{
    bool ret;
    // last H(X), T(X,X'), T(X',X'',...), B(X',X'',...)
    // DEBUG: this takes too much time
    if (PROF_SAT(sat(cumu_hypt & trans_hd & badp, m)))
        ret = true;
    else
        ret = false;
    return ret;
}

bool CoiMana::soundness (Bf_ptr cdnf)
{
    m.releaseSw(tent_sw);
    tent_sw = m.newSw();

    auto first_cdnf  = subst(cdnf, first_state_varmap);
    auto second_cdnf = subst(cdnf, second_state_varmap);
    frnt  = v(addBf(first_cdnf,  m, tent_sw));
    frntp = v(addBf(second_cdnf, m, tent_sw));

    bool ret;
    // H(X'), T(X',X'',...) => ~B(X',X'',...) not hold
    // H(X'), B(X',X'',...)) sat
    // H(X'), B(X',X'',...)) sat
    if (PROF_SAT(sat(frntp & badp, m)))
    {
        ret = true;
        ce = mk_ce(second_state_varmap, m);
    }
    else
    {
        ret = false;
        frnt = v(addBf(first_cdnf,  m, cumu_sw));
    }
    return ret;
}

Bv_ptr CoiMana::get_ce ()
{
    assert(ce);
    return ce;
}

//=================================================================================================
}

#undef SAT_NAME_ID_TRAIL
#undef PROF_SAT_AS
#undef PROF_SAT
#undef COUNT_CLAUSE
#ifdef COUNTING
#undef addBf
#undef addAig
#endif
