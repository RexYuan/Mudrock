
#include "sig.hxx"

bool terminate_requested = false;

void sigterm_handler (int signum)
{
    assert(SIGTERM == signum);
    terminate_requested = true;
}
