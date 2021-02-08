
#include "minisat.hxx"

#include "bv.hxx"
#include "mana.hxx"
#include "bf.hxx"

using std::cout;
using namespace Minisat;

struct S
{
    S ()
    {
        cout << "created" << '\n';
    }
    ~S ()
    {
        cout << "deleted" << '\n';
    }
};

int main()
{
    cout << std::boolalpha;

    
    Bf_ptr x = v(1) & v(2);
    cout << (x & ~x) << '\n';
    x = v(1) | v(2);
    cout << (x | ~x) << '\n';

    return 0;
}
