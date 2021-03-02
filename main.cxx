
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
    AigLit g{true,2};
    cout << (bool)g;
}
