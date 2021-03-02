
#include "minisat.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "mana.hxx"
#include "comm.hxx"

#include <iostream>
using std::cout;

int main()
{
    Mana m{};
    Var v1 = m.newVar(), v2 = m.newVar();
    Var sw = m.newSw();
    Var bf_v = addBf(v(v1) | v(v2), m);
    m.addClause(sw, mkLit(bf_v));
    assert(sat(v(v1) & v(v2), m));
    assert(hold(~(~v(v1) & ~v(v2)), m));
}
