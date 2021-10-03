
#include "more_mana.hxx"

namespace
{
    // add `bf` to `m` on `tmpSw` and return `tmpSw`
    Sw assume (const Bf_ptr& bf, Mana& m)
    {
        Var tmpSw = m.newSw();
        Var bfVar = addBf(bf, m, tmpSw);
        m.addClause(tmpSw, mkLit(bfVar));
        return tmpSw;
    }
}

bool sat (const Bf_ptr& bf, Mana& m)
{
    Sw tmpSw = assume(bf, m);
    bool ret = m.solve();
    m.releaseSw(tmpSw);
    return ret;
}

bool hold (const Bf_ptr& bf, Mana& m)
{
    return !sat(~bf, m);
}

bool evaluate (Mana& m, const Bv_ptr valuation, const vector<Var>& state_varmap)
{
    assert(valuation->len() == state_varmap.size());

    vec<Lit> ps;
    auto bit = --valuation->end();
    auto mit = --state_varmap.end();
    for (; bit != --valuation->begin(); bit--, mit--)
    {
        if (*bit) ps.push(mkLit(*mit));
        else ps.push(~mkLit(*mit));
    }
    return m.solve(ps);
}

bool evaluate (const Bf_ptr& bf, Mana& m, const Bv_ptr valuation, const vector<Var>& state_varmap)
{
    Sw tmpSw = assume(bf, m);
    bool ret = evaluate(m, valuation, state_varmap);
    m.releaseSw(tmpSw);
    return ret;
}

// TODO: rework tabulate with vector varmap
/*
namespace
{
    inline void validate_range (const Mana& m, const vector<Var>& range)
    {
        for ([[maybe_unused]] const auto& v: range)
            assert(v < m.nVars());
    }

    inline map<int,int> mk_index_varmap (const vector<Var>& range)
    {
        map<int,int> tmp;
        for (size_t i=0; const auto v : range)
            tmp[i++] = v;
        return tmp;
    }
}
string tabulate (Mana& m, vector<Var> range)
{
    validate_range(m, range);
    auto index_varmap = mk_index_varmap(range);

    string table = "listing truth table:\n"s;
    size_t line_size = range.size() + " 0\n"s.size(),
           line_num  = pow(2, range.size());
    table.reserve(table.size() + line_size * line_num);

    size_t num_var=range.size();
    for (unsigned i=0; i<pow(2,num_var); i++)
    {
        auto tmp = Bv{num_var, i};
        table += tmp.to_string();
        table += ' ';

        auto ret = evaluate(m, tmp, index_varmap);

        table += to_string(static_cast<unsigned>(ret));
        table += '\n';
    }

    return table;
}
*/
