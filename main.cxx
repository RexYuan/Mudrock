
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
    Face f{Bv{"11"}};
    f.push(Bv{"10"});
    f.push(Bv{"01"});
    auto f_bf = toBf(f);
    cout << f_bf << "\n";
}
