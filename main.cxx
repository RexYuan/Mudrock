
#include "minisat.hxx"

#include "bv.hxx"
#include "mana.hxx"
#include "bf.hxx"
#include "ora.hxx"

using std::cout;
using namespace Minisat;

int main()
{
    //cout << std::boolalpha;

    Bv b ("11110000");

    assert (b.getter(0) == true);
    assert (b.getter(1) == true);
    assert (b.getter(2) == true);
    assert (b.getter(3) == true);
    assert (b.getter(4) == false);
    assert (b.getter(5) == false);
    assert (b.getter(6) == false);
    assert (b.getter(7) == false);

    cout << b.to_string() << '\n';

    b.setter(0, false);
    b.flipper(3);
    b.setter(7, 1);

    assert (b.getter(0) == false);
    assert (b.getter(1) == true);
    assert (b.getter(2) == true);
    assert (b.getter(3) == false);
    assert (b.getter(4) == false);
    assert (b.getter(5) == false);
    assert (b.getter(6) == false);
    assert (b.getter(7) == true);
    
    cout << b.to_string() << '\n';

    for (auto i : b) cout << i;
    cout << '\n';

    for (auto it=b.begin(); it!=b.end(); ++it) it.setbit(!*it);

    assert (b.getter(0) == true);
    assert (b.getter(1) == false);
    assert (b.getter(2) == false);
    assert (b.getter(3) == true);
    assert (b.getter(4) == true);
    assert (b.getter(5) == true);
    assert (b.getter(6) == true);
    assert (b.getter(7) == false);

    cout << b.to_string() << '\n';
    return 0;


#if 0
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
#endif
}