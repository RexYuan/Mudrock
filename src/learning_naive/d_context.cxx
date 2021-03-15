
#include "d_context.hxx"

D_Context::D_Context (string filename) :
teacher{D_Teacher{filename}},
learner{D_Learner{teacher}} {}

void D_Context::check ()
{
    learner.learn();
    result = learner.result();
}

bool D_Context::sat () const
{
    switch (result)
    {
    case Refuted: return true;
    case Perfect: return false;
    default: break;
    }
    assert(false);
}
