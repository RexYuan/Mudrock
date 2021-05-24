
#include "direct_context.hxx"

namespace Direct
{
//=================================================================================================
Context::Context (string filename) :
teacher{Teacher{filename}},
learner{Learner{teacher}} {}

void Context::check ()
{
    learner.learn();
    result = learner.result();
}

Feedback Context::sat () const
{
    switch (result)
    {
    case Refuted: [[fallthrough]];
    case Perfect: return result;
    default: break;
    }
    throw InvalidFeedback("Must be final and determined."s);
}
//=================================================================================================
}
