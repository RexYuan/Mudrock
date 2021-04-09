
#pragma once

#include <cassert>
#include <cstdlib>
#include <csignal>

extern bool terminate_requested;
void sigterm_handler (int signum);
