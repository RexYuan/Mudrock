
#include "b_context.hxx"

B_Context::B_Context (string filename) :
teacher{B_Teacher{filename}},
learner{teacher} {}

B_Types::Feedback B_Context::check ()
{
    teacher.setup();
    if (teacher.degen())
        return (result = Refuted);
    learner.kickoff();
    while (true)
    {
        if (!teacher.advanceable())
        {
            teacher.unroll();
            if (teacher.degen())
                return (result = Refuted);
            else
            {
                teacher.restart();
                // only restart learner at unroll
                if (teacher.advanceable())
                    learner.relearn();
            }
        }
        else
        {
            // persistent learning
            learner.learn();
            if (!teacher.progressed())
                return (result = Perfect);
            else
                teacher.advance();
        }
    }
}

bool B_Context::sat () const
{
    switch (result)
    {
    case Refuted: return true;
    case Perfect: return false;
    default: break;
    }
    assert(false);
}
