
#include "donut_context.hxx"

namespace Donut
{
//=================================================================================================
Context::Context (string filename) :
teacher{Teacher{filename, prof.tprof}},
learner{teacher} {}

Feedback Context::check ()
{
    teacher.setup();                             // last frnt = false, frnt = init
    if (teacher.degen())                         // if init meets bad
        return (result = Refuted);               //     return refuted
    learner.learn();                             // frnt = frnt image < frnt < bad
    while (true)                                 // loop
    {                                            // {
        if (!teacher.advanceable())              //     if frnt image meets bad
        {                                        //     {
            teacher.unroll();                    //         unroll bad
            if (teacher.degen())                 //         if init meets bad
                return (result = Refuted);       //             return refuted
            else                                 //         else
                teacher.restart();               //             frnt = init
        }                                        //     }
        else                                     //     else
        {                                        //     {
            learner.relearn();                   //         frnt = frnt image < frnt < bad
            if (!teacher.progressed())           //         if frnt <= last frnt
                return (result = Perfect);       //             return perfect
            else                                 //         else
                teacher.advance();               //             last frnt = frnt
        }                                        //     }
    }                                            // }
}

bool Context::sat () const
{
    switch (result)
    {
    case Refuted: return true;
    case Perfect: return false;
    default: break;
    }
    assert(false);
}
//=================================================================================================
}