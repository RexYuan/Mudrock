
#include "minisat.hxx"

#include "bv.hxx"
#include "mana.hxx"
#include "bf.hxx"
#include "ora.hxx"

using std::cout;
using namespace Minisat;

int main()
{
    cout << std::boolalpha;

    size_t le = 2;
    Ora o(le);
    o.addState();

    assert(o.sat(v(0) != v(1), vector<size_t>{0}));
    assert(o.hold(v(0) == v(0), vector<size_t>{0}));

    auto to_bs = [le=le](unsigned num) -> string
    {
        string bs;
        for (size_t i=0; i<le; i++, num>>=1)
            if (1&num) bs = '1'+bs;
            else bs = '0'+bs;
        return bs;
    };
    cout << "eval\n";
    for (unsigned i=0; i<pow(2,le); i++)
        cout << Bv{to_bs(i)} << ' ' << (int)o.eval (Bv{to_bs(i)}, v(0) != v(1), vector<size_t>{0}, vector<size_t>{0}) << '\n';

    cout << "assume\n";
    o.assume(v(0) != v(1), vector<size_t>{0});
    cout << o.tabulation();

    cout << "eval\n";
    for (unsigned i=0; i<pow(2,le); i++)
        cout << Bv{to_bs(i)} << ' ' << (int)o.eval (Bv{to_bs(i)}, vector<size_t>{0}) << '\n';

    cout << "refresh\n";
    o.refresh();
    cout << o.tabulation();

    cout << "insist\n";
    o.insist(v(0) != v(1), vector<size_t>{0});
    cout << o.tabulation();

    cout << "refresh\n";
    o.refresh();
    cout << o.tabulation();

    cout << "reset\n";
    o.reset();
    cout << o.tabulation();

    cout << "persist\n";
    o.persist(v(0) != v(1), vector<size_t>{0});
    cout << o.tabulation();

    cout << "refresh\n";
    o.refresh();
    cout << o.tabulation();
    
    cout << "reset\n";
    o.reset();
    cout << o.tabulation();

    o.addState();
    cout << "addState\n";
    o.assume(v(0) != v(1), vector<size_t>{1});
    cout << o.tabulation();

    return 0;
}
