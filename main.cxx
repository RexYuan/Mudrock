
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
    Bv p (5, 5);
    assert(b == p);
}
