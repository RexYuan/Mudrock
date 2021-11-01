
#include "donut_context.hxx"

namespace Donut
{
//=================================================================================================
Context::Context (string filename) :
teacher{Teacher{filename}},
learner{teacher} {}

Feedback Context::check ()
{
    if (teacher.degen())
        return (result = Refuted);
    teacher.setup();                             // prepare constraints
    teacher.restart();                           // prepare frontiers: last frnt = false, frnt = init
    if (teacher.reachbad())                         // if init meets bad
        return (result = Refuted);               //     return refuted
    learner.learn();                             // frnt = frnt image < frnt < bad
    size_t depth = 0;
    while (true)                                 // loop
    {                                            // {
        if (teacher.meetbad())              //     if frnt image meets bad
        {                                        //     {
            log(3, "Top", "Restarting, iter depth = "s + to_string(depth));
            depth=0;
            teacher.unroll();                    //         unroll bad
            if (teacher.reachbad())                 //         if init meets bad
                return (result = Refuted);       //             return refuted
            else                                 //         else
                teacher.restart();               //             frnt = init
        }                                        //     }
        else                                     //     else
        {                                        //     {
            learner.relearn();                   //         frnt = frnt image < frnt < bad
            if (teacher.fixedpoint())           //         if frnt <= last frnt
            {
                log(3, "Top", "Ending, iter depth = "s + to_string(depth));
                return (result = Perfect);       //             return perfect
            }
            else                                 //         else
            {
                teacher.advance();               //             last frnt = frnt
                depth++;
            }
        }                                        //     }
        if (terminate_requested)
        {
            log(3, "Top", "SIGTERM received. Check returning Unknown");
            return (result = Unknown);
        }
    }                                            // }
}

Feedback Context::sat () const
{
    switch (result)
    {
    case Refuted: [[fallthrough]];
    case Perfect: [[fallthrough]];
    case Unknown: return result;
    default: break;
    }
    throw InvalidFeedback("Must be final."s);
}

string Context::witness () const
{
    stringstream wit;
    wit << "1\n"s
        << "b0\n"s
        << string(teacher.aig.num_latches(), '0') << "\n"s;
    for (const Bv_ptr line : teacher.witness())
        wit << line << "\n"s;
    wit << ".\n"s;
    return wit.str();
}
//=================================================================================================
}
