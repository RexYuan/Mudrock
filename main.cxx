
#include "minisat.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "mana.hxx"
#include "ora.hxx"

using std::cout;
using namespace Minisat;

#include <type_traits>
#include <cstring>
#include <utility>
#include <exception>
#include <string>

int main()
{
    Bf_ptr x = v(true);
    x &= v(1);
    x = x & v(2);
    cout << (x & ~x) << '\n';
    x = v(1);
    x |= v(false);
    x |= v(2);
    cout << (x | ~x) << '\n';
    map<int,int> to;
    to[1] = 4; to[2] = 3;
    x = subst(x, to);
    cout << (x | ~x) << '\n';
    to[4] = 3; to[3] = 4;
    cout << subst(x | ~x, to) << '\n';
}
