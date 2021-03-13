
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

#include "more_bf.hxx"

inline Bf_ptr mk_cdnf (const vector<Face>& faces)
{
    Bf_ptr tmp = v(true);
    for (const auto& face : faces)
        tmp = tmp & toBf(face);
    return tmp;
}

int main()
{
    Face f1{Bv{"11"}}, f2{Bv{"00"}};
    f1.push(Bv{"01"}); f2.push(Bv{"01"});
    f1.push(Bv{"10"}); f2.push(Bv{"10"});
    vector<Face> tmp;
    tmp.push_back(move(f1)); tmp.push_back(move(f2));
    Bf_ptr cdnf = mk_cdnf(tmp);

    assert(contains(cdnf, 0));
    assert(contains(cdnf, 1));
    assert(!contains(cdnf, 2));

    assert(false == evaluate(cdnf, Bv{"00"}));
    assert(true  == evaluate(cdnf, Bv{"01"}));
    assert(true  == evaluate(cdnf, Bv{"10"}));
    assert(false == evaluate(cdnf, Bv{"11"}));

    cout << tabulate(cdnf, 2);
}
