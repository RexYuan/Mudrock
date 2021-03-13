
#include <cassert>

#include "minisat.hxx"

#include <vector>
using std::vector;
#include "d_types.hxx"
#include "d_teacher.hxx"
#include "face.hxx"
#include "bv.hxx"

#include <iostream>
using std::cout;

int main()
{
    // start=00, bad=11
    D_Teacher t{"aag/test.aag"};

    vector<Face> degen_true{};
    assert(D_Types::Feedback::TooBig == t.consider(degen_true));

    vector<Face> degen_false{};
    degen_false.emplace_back(Bv{"11"});
    assert(D_Types::Feedback::TooSmall == t.consider(degen_false));

    // one step before refutation
    vector<Face> tmp;
    tmp.emplace_back(Bv{"11"});
    tmp[0].push(Bv{"00"});
    assert(D_Types::Feedback::Refuted == t.consider(tmp));
}
