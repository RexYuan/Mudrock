
#pragma once

#include <cassert>

#include <string>
using namespace std::string_literals;
using std::string;

#include <iostream>
using std::ostream;

namespace Donut
{
    enum class Feedback {Refuted, Perfect, TooBig, TooSmall, Unknown};
}

ostream& operator << (ostream& out, const Donut::Feedback& feedback);
