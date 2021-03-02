
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
    Face f{Bv{"1001"}};
    assert(f.empty());
    f.push(Bv{"1111"});
    assert(f.size() == 1);
}
