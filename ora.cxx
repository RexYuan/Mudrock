
#include "ora.hxx"

Ora::Ora (size_t length) : l{length}, primarySw{m.newSw()}, secondarySw{m.newSw()} {}

//=================================================================================================
// Main interface
//
bool Ora::sat (const Bf_ptr& bf, optional<vector<size_t>> space)
{
    Var tmpSw = m.newSw();
    bool ret = m.solve(mkLit(importBf(tmpSw, bf, space)));
    m.releaseSw(tmpSw);
    return ret;
}

bool Ora::hold (const Bf_ptr& bf, optional<vector<size_t>> space)
{
    return !sat(~bf, space);
}

void Ora::assume (const Bf_ptr& bf, optional<vector<size_t>> space)
{
    Var bf_v = importBf (primarySw, bf, space);
    m.addClause(primarySw, mkLit(bf_v));
}

void Ora::insist (const Bf_ptr& bf, optional<vector<size_t>> space)
{
    Var bf_v = importBf (secondarySw, bf, space);
    m.addClause(secondarySw, mkLit(bf_v));
}

void Ora::persist (const Bf_ptr& bf, optional<vector<size_t>> space)
{
    Var bf_v = importBf (m.fixedSw(), bf, space);
    m.addClause(m.fixedSw(), mkLit(bf_v));
}

bool Ora::eval (const Bv& bv, optional<vector<size_t>> bv_space)
{
    if (bv_space) assert(bv.len() == bv_space.value().size()*l);
    
    vec<Lit> vals;
    for (size_t i=0; auto b : bv)
    {
        if (b) vals.push(mkLit(Ind2Var(i, bv_space)));
        else vals.push(~mkLit(Ind2Var(i, bv_space)));
        i++;
    }

    return m.solve(vals);
}

bool Ora::eval (const Bv& bv, const Bf_ptr& bf, optional<vector<size_t>> bv_space, optional<vector<size_t>> bf_space)
{
    Var tmpSw = m.newSw();
    m.addClause(tmpSw, mkLit(importBf(tmpSw, bf, bf_space)));
    bool ret = eval(bv, bv_space);
    m.releaseSw(tmpSw);
    return ret;
}

//=================================================================================================
// Command
//
void Ora::refresh ()
{
    m.releaseSw(primarySw);
    primarySw = m.newSw();
}

void Ora::reset ()
{
    m.releaseAll();
    primarySw = m.newSw();
    secondarySw = m.newSw();
}

//=================================================================================================
// State management
//
size_t Ora::nStates ()
{
    return states.size();
}

size_t Ora::addState ()
{
    vector<Var> tmp;
    for (size_t i=0; i<l; i++)
        tmp.push_back(m.newVar());
    states.push_back(tmp);
    return nStates();
}

//=================================================================================================
// Stats
//
string Ora::tabulation ()
{
    assert(nStates() > 0);
    string table = "listing truth table:\n";

    auto to_bs = [&](unsigned num) -> string
    {
        string bs;
        for (size_t i=0; i<l; i++, num>>=1)
            if (1&num) bs = '1'+bs;
            else bs = '0'+bs;
        return bs;
    };
    auto baseHelper = [&](string bss) -> void
    {
        vec<Lit> tmp;
        for (size_t n=0; n<nStates(); n++)
        for (size_t i=0; i<l; i++)
            if (bss[l*n+i]=='1') tmp.push(mkLit(states[n][i]));
            else tmp.push(~mkLit(states[n][i]));

        for (size_t n=1; n<nStates(); n++)
            bss.insert(l*n, " ");
        table += bss; table += ' '; table += (m.solve(tmp)?'1':'0'); table += '\n';
    };
    function<void(string,unsigned int)> recurHelper;
    recurHelper = [l=l,&to_bs,&baseHelper,&recurHelper](string prefix, size_t depth) -> void
    {
        if (depth == 0)
            baseHelper(prefix);
        else
            for (unsigned i=0; i<pow(2,l); i++)
                recurHelper(prefix+to_bs(i), depth-1);
    };

    recurHelper("", nStates());
    return table;
}

//=================================================================================================
// Internal utilities
//
Var Ora::importBf (Var sw, const Bf_ptr& bf, optional<vector<size_t>> spaceopt)
{
    switch (bf->t)
    {
    case Conn::Top: return m.constTrue();
    case Conn::Bot: return m.constFalse();
    case Conn::Base: return Ind2Var(bf->get_int(), spaceopt);
    case Conn::Not:
    {
        Var v = m.newVar();
        Var sub_v = importBf(sw, bf->get_sub(), spaceopt);
        m.addClause(sw,  mkLit(v),  mkLit(sub_v));
        m.addClause(sw, ~mkLit(v), ~mkLit(sub_v));
        return v;
    }
    case Conn::And:
    {
        Var v = m.newVar();
        vec<Lit> ps; ps.push(mkLit(v));
        for (Var sub_v; Bf_ptr sub : bf->get_subs())
        {
            sub_v = importBf(sw, sub, spaceopt);
            m.addClause(sw, ~mkLit(v), mkLit(sub_v));
            ps.push(~mkLit(sub_v));
        }
        m.addClause(sw, ps);
        return v;
    }
    case Conn::Or:
    {
        Var v = m.newVar();
        vec<Lit> ps; ps.push(~mkLit(v));
        for (Var sub_v; Bf_ptr sub : bf->get_subs())
        {
            sub_v = importBf(sw, sub, spaceopt);
            m.addClause(sw, mkLit(v), ~mkLit(sub_v));
            ps.push(mkLit(sub_v));
        }
        m.addClause(sw, ps);
        return v;
    }
    }
    assert(false);
}

Var Ora::Ind2Var (int ind, optional<vector<size_t>> spaceopt)
{
    /*
     * to the user, the state space is contiguous
     * example: space={1,2}
     *         0       1
     * ind     0 1 2 / 3 4 5
     * space   1       2
     *         3 4 5 / 6 7 8
     * `4` in `bf` maps to the `2` in `space` and in turn to `7` in `states[2]`
     */
    if (spaceopt) assert((ind / l) < spaceopt.value().size());

    if (spaceopt) return states[spaceopt.value()[ind / l]][ind % l];
    else return ind;
}
