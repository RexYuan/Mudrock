
#include "minisat.hxx"

#include "bv.hxx"
#include "mana.hxx"

using namespace std;
using namespace Minisat;

int main()
{
    cout << boolalpha;
    Mana m;

    Var s1 = m.newSw();
    Var x = m.newVar(), y = m.newVar(), z = m.newVar();

    m.addClause(s1, mkLit(x));

    m.solve(mkLit(y), mkLit(z));
    assert (m.sat());

    assert (m.val(x) == true);
    assert (m.val(y) == true);
    assert (m.val(z) == true);

    m.releaseSw(s1);
    
    Var s2 = m.newSw();
    LitVec ls1, ls2;
    
    ls1.push(mkLit(y));
    ls1.push(mkLit(z));
    m.addClause(s2, ls1);

    ls2.push(~mkLit(y));
    ls2.push(~mkLit(z));
    m.addClause(s2, ls2);

    LitVec assump;
    assump.push(mkLit(x));
    assump.push(~mkLit(z));
    m.addClause(s2, assump);

    m.solve(assump);
    assert (m.sat());

    assert (m.val(x) == true);
    assert (m.val(y) == true);
    assert (m.val(z) == false);

    assert (m.nVars() == 5);

    m.releaseSw(s2);
    m.releaseVar(x);
    m.releaseVar(y);
    m.releaseVar(z);

    return 0;
}
