
#include <cassert>

#include "b_types.hxx"
#include "b_teacher.hxx"
#include "b_learner.hxx"
#include "b_context.hxx"

#include <iostream>
using std::cout;

#include <chrono>
void timer ()
{
    static auto start = std::chrono::steady_clock::now();
    static auto run = 0;

    if (run & 1)
    {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << elapsed_seconds.count() << "s";
    }
    else
    {
        start = std::chrono::steady_clock::now();
    }

    run++;
    return;
}

bool test(string filename, bool sat)
{
    string tmp = "aag/";
    tmp += filename;
    B_Context c{tmp};
    cout << "#" << c.teacher.aig.num_latches();

    cout << " <" << filename << ">" << std::flush << " (";
    timer();
    c.check();
    timer();
    cout << ") : ";

    if (c.sat() == sat)
        cout << "passed\n";
    else
        cout << "failed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

    cout << std::flush;
    return c.sat() == sat;
}

int main()
{
    assert(test("linear.aag", true));
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
    //assert(test("bobcount.aag", false));
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
    //assert(test("viseisenberg.aag", true));
    //assert(test("bjrb07amba1andenv.aag", false));
    // 23
    //assert(test("eijks208c.aag", false));
    assert(test("visarbiter.aag", false));
    //assert(test("vis4arbitp1.aag", false));
    // 24
    //assert(test("pdtpmsudc8.aag", false));
    // 25
    //assert(test("ringp0.aag", true));
    //assert(test("ringp0neg.aag", true));
    //assert(test("visbakery.aag", true));
}
