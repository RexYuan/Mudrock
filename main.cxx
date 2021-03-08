
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
    Bv b ("00101");
    auto bit = b.begin();
    for (; bit!=b.end(); bit++)
        cout << *bit;
    cout << "\n";
    assert(bit == b.end());

    for (bit--; bit!=(--b.begin()); bit--)
        cout << *bit;
    cout << "\n";
    assert(bit == (--b.begin()));
}
