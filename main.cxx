
#include "minisat.hxx"

#include "aig.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "mana.hxx"
#include "ora.hxx"

#include <iostream>
using std::cout;

int main()
{
    Bv b ("1010101");
   
    cout << b.to_string() << "\n";

    for (auto it=b.begin(); it!=b.end();) cout << *(++it);
    cout << "\n";
    for (auto it=b.begin(); it!=b.end();) cout << *(it++);
    cout << "\n";
}
