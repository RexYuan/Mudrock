
#include "sig.hxx"

volatile sig_atomic_t terminate_requested = false;

void sigterm_handler (int signum)
{
    assert(SIGTERM == signum);
    terminate_requested = true;
}
