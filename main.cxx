
#include "minisat.hxx"

#include "bv.hxx"

using namespace std;

int main()
{

    Bv b ("11110000");

    cout << b.getter(0) << " " << b.getter(3) << " " << b.getter(7) << endl;
    b.setter(0, false);
    b.flipper(3);
    b.setter(7, 1);
    cout << b.getter(0) << " " << b.getter(3) << " " << b.getter(7) << endl;
    
    cout << b.to_string() << endl;

    for (auto i : b) cout << i;
    cout << endl;

    for (auto it=b.begin(); it!=b.end(); ++it) it.setbit(!*it);
    cout << b.to_string() << endl;

    return 0;
}
