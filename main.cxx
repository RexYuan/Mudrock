
#include "minisat.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "mana.hxx"

#include <iostream>
using std::cout;

int main()
{
    Mana m{};
    Var v = m.newVar();
    m.addClause(m.fixedSw(), mkLit(v));
    m.solve();
    assert(m.val(v));
}
