
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

bool Context::sat () const
{
    switch (result)
    {
    case Refuted: return true;
    case Perfect: return false;
    default: break;
    }
    throw InvalidFeedback("Must be final and determined."s);
}
//=================================================================================================
}
