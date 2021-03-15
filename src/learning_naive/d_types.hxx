
#pragma once

#include <cassert>

#include <string>
using namespace std::string_literals;
using std::string;

#include <iostream>
using std::ostream;

namespace D_Types
{
    enum class Feedback {Refuted, Perfect, TooBig, TooSmall, Unknown};
}

ostream& operator << (ostream& out, const D_Types::Feedback& feedback);
