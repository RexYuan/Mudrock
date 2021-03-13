
#include <cassert>

#include "minisat.hxx"

#include "d_teacher.hxx"
#include "face.hxx"
#include "bv.hxx"
#include <utility>
using std::move;

#include <iostream>
using std::cout;

int main()
{
    cout << toBf(Bv{"10101"}) << "\n";
}
