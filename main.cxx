
#include <cassert>

#include "m_types.hxx"
#include "m_teacher.hxx"
#include "m_learner.hxx"
#include "m_context.hxx"

#include <iostream>
using std::cout;

bool test(string filename, bool sat)
{
    string tmp = "aag/";
    tmp += filename;
    M_Context c{tmp};
    c.check();
    return c.sat() == sat;
}

#include "more_mana.hxx"
#include "to_minisat.hxx"
#include <vector>
using std::vector;
#include "face.hxx"
#include "bf.hxx"
#include "bv.hxx"
#include <utility>
using std::move;

int main()
{
    M_Teacher t{"aag/linear.aag"};
    // 00 -> 01 -> 10 -> 11
    // ^init             ^bad
    t.setup();
    assert(!t.degen());

    vector<Face> tmp{};

    assert(t.advanceable());
    tmp.emplace_back(Bv{"11"});
    tmp[0].push(Bv{"00"}); // 00
    // induction forces 00
    // progress forces 01
    // soundness disallows 11
    assert(t.advanceable());
    assert(M_Types::Feedback::TooSmall == t.consider(tmp));
    assert(Bv{"01"} == t.counterexample());
    tmp[0].push(Bv{"01"}); // 00 01
    assert(M_Types::Feedback::Perfect == t.consider(tmp));
    t.advance();
    // induction forces 00 01
    // progress forces 10
    // soundness disallows 11
    assert(t.advanceable());
    assert(M_Types::Feedback::TooSmall == t.consider(tmp));
    assert(Bv{"10"} == t.counterexample());
    tmp[0].push(Bv{"10"}); // 00 01 10
    assert(M_Types::Feedback::Perfect == t.consider(tmp));
    t.advance();
    // induction forces 00 01 10
    // progress forces 11
    // soundness disallows 11
    assert(!t.advanceable());

    t.unroll();
    assert(!t.degen());
    t.restart();
    tmp.clear();
    // induction forces 00
    // progress forces 01
    // soundness disallows 10 11
    assert(t.advanceable());
    assert(M_Types::Feedback::TooBig == t.consider(tmp));
    assert(Bv{"10"} == t.counterexample());
    tmp.emplace_back(Bv{"10"});
    assert(M_Types::Feedback::TooSmall == t.consider(tmp));
    assert(Bv{"00"} == t.counterexample());
    tmp[0].push(Bv{"00"});
    assert(M_Types::Feedback::Perfect == t.consider(tmp));
    t.advance();
    // induction forces 00 01
    // progress forces 10
    // soundness disallows 10 11
    assert(!t.advanceable());

    t.unroll();
    assert(t.degen());

    return 0;

    /*assert(test("linear.aag", true));
    // state size
    // 3
    assert(test("bj08aut1.aag", false));
    assert(test("bj08aut5.aag", false));
    assert(test("bj08aut62.aag", false));
    assert(test("bj08aut82.aag", false));
    // 5
    assert(test("bj08autg3f1.aag", true));
    assert(test("bj08autg3f2.aag", true));
    assert(test("bj08autg3f3.aag", true));
    assert(test("pdtvisgray0.aag", false));
    assert(test("pdtvisgray1.aag", false));
    // 10
    assert(test("pdtvispeterson.aag", false));
    assert(test("nusmvsyncarb5p2.aag", false));
    // 13
    assert(test("bobtutt.aag", true));
    assert(test("bobcount.aag", false));
    // 14
    assert(test("shortp0.aag", true));
    assert(test("shortp0neg.aag", true));
    // 15
    assert(test("visemodel.aag", false));
    // 16
    assert(test("counterp0.aag", true));
    assert(test("eijks208o.aag", false));
    assert(test("counterp0neg.aag", true));
    assert(test("pdtvisgigamax0.aag", false));
    assert(test("pdtvisgigamax1.aag", false));
    assert(test("pdtvisgigamax2.aag", false));
    assert(test("pdtvisgigamax3.aag", false));
    assert(test("pdtvisgigamax4.aag", false));
    assert(test("pdtvisgigamax5.aag", false));
    // 20
    assert(test("mutexp0.aag", true));
    assert(test("mutexp0neg.aag", true));
    //assert(test("bobuns2p10d20l.aag", false));
    assert(test("nusmvsyncarb10p2.aag", false));
    // 21
    assert(test("neclaftp5001.aag", false));
    assert(test("neclaftp5002.aag", false));
    // 22
    assert(test("eijks208.aag", false));
    assert(test("viseisenberg.aag", true));
    assert(test("bjrb07amba1andenv.aag", false));
    // 23
    assert(test("eijks208c.aag", false));
    assert(test("visarbiter.aag", false));
    //assert(test("vis4arbitp1.aag", false));
    // 24
    //assert(test("pdtpmsudc8.aag", false));
    // 25
    assert(test("ringp0.aag", true));
    //assert(test("ringp0neg.aag", true));
    //assert(test("visbakery.aag", true));*/
}
