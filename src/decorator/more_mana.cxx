
#include "more_mana.hxx"

bool evaluate (Mana& m, const Bv& valuation, const vector<Var> vars)
{
    assert(valuation.len() == vars.size());

    auto bit = valuation.begin();
    auto vit = vars.begin();

    vec<Lit> ps;
    for (; bit!=valuation.end(); bit++, vit++)
    {
        if (*bit) ps.push(mkLit(*vit));
        else ps.push(~mkLit(*vit));
    }
    return m.solve(ps);
}

namespace
{
    inline void validate_range (const Mana& m, const vector<Var>& range)
    {
        for (const auto& v: range)
            assert(v < m.nVars());
    }
}
string tabulate (Mana& m, vector<Var> range)
{
    validate_range(m, range);
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

        auto ret = evaluate(m, tmp, range);

        table += to_string(static_cast<unsigned>(ret));
        table += '\n';
    }

    return table;
}

