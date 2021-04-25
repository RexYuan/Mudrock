
#pragma once

#include <cassert>
#include <cstdlib>
#include <csignal>

extern volatile sig_atomic_t terminate_requested;
void sigterm_handler (int signum);
