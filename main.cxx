
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
    D_Teacher t{"aag/test.aag"};

    vector<Face> degen_true{};
    vector<Face> degen_false{};
    degen_false.emplace_back(Bv{"000"});

    assert(D_Teacher::Feedback::TooBig == t.consider(degen_true));
    assert(D_Teacher::Feedback::TooSmall == t.consider(degen_false));
}
