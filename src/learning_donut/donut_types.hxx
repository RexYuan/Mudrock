
#pragma once

#include <cassert>

#include <string>
using namespace std::string_literals;
using std::string;

#include <iostream>
using std::ostream;
#include <stdexcept>
using std::logic_error;

namespace Donut
{
    enum class Feedback {Refuted, Perfect, TooBig, TooSmall, Unknown};

    struct InvalidFeedback : public logic_error
    {
        InvalidFeedback (const string& what_arg) : logic_error(what_arg) {}
    };
}

ostream& operator << (ostream& out, const Donut::Feedback& feedback);
