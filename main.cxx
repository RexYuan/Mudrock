
#include "minisat.hxx"

#include "bv.hxx"
#include "mana.hxx"
#include "bf.hxx"
#include "ora.hxx"
#include "face.hxx"

using std::cout;
using namespace Minisat;

#include <type_traits>
#include <cstring>
#include <utility>
#include <exception>
#include <string>

int main()
{
    cout << std::boolalpha;

    Face f{Bv{"0000"}};
    Face f2{move(f)};
    cout << f2 << '\n';
    Bv lo{"1111"};
    f2.push(move(lo));
    cout << f2 << '\n';
    f2.push(Bv{"0111"});
    cout << f2 << '\n';
    f2.push(Bv{"1110"});
    cout << f2 << '\n';
    f2.push(Bv{"0110"});
    cout << f2 << '\n';

    assert(f2(Bv{"1111"}));
    assert(f2(Bv{"1110"}));
    assert(f2(Bv{"0111"}));
    assert(f2(Bv{"0110"}));
    assert(!f2(Bv{"1101"}));

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
