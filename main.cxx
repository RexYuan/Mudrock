
#include "minisat.hxx"

#include "bv.hxx"
#include "mana.hxx"
#include "bf.hxx"

using std::cout;
using namespace Minisat;

int main()
{
    cout << std::boolalpha;

    
    Bf_ptr x = v(1) & v(2);
    Bf_ptr y = v(3) | v(4);
    
    cout << (x |= ~v(true) != y == ~v(false)) << '\n';

    return 0;
}
