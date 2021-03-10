
#include "minisat.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "mana.hxx"
#include "to_minisat.hxx"
#include "to_bf.hxx"

#include <iostream>
using std::cout;

int main()
{
    Mana m;
    auto v1 = m.newVar();
    auto v2 = m.newVar();
    auto v3 = m.newVar();

    fixBf(v(v1) != v(v2), m);
    fixBf(v(v2) != v(v3), m);

    assert(hold(v(v1) == v(v3), m));
}
