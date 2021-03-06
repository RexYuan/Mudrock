
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
    Mana m;
    Aig aig{"aag/pdtvisgray0.aag"};
    auto varmap = addAig(aig, m);
    for (const auto& [aigvar,var] : varmap)
        cout << aigvar << " : " << var << "\n";
}
