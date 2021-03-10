
#include "minisat.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "mana.hxx"
#include "to_minisat.hxx"
#include "to_bf.hxx"
#include "more_mana.hxx"

#include <iostream>
using std::cout;

int main()
{
    Mana m;
    Aig aig{"aag/test.aag"};
    auto varmap1 = addAig(aig, m);
    auto varmap2 = addAig(aig, m);

    vector<Var> tmp;
    for (auto [aigvar,aiglit] : aig.latches())
        tmp.push_back(varmap1[aigvar]);
    for (auto [aigvar,aiglit] : aig.latches())
        tmp.push_back(varmap2[aigvar]);

    for (auto [aigvar,aiglit] : aig.latches())
    {
        Bf_ptr next = toBf(aigvar),
               curr = toBf(aiglit);

        next = subst(next, toBfmap(varmap2));
        curr = subst(curr, toBfmap(varmap1));

        fixBf(next == curr, m);
    }

    cout << tabulate(m, tmp);
}
