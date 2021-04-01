
#include "donut_types.hxx"

ostream& operator << (ostream& out, const Donut::Feedback& feedback)
{
    switch (feedback)
    {
        case Donut::Feedback::Refuted:  out << "Refuted"s;  break;
        case Donut::Feedback::Perfect:  out << "Perfect"s;  break;
        case Donut::Feedback::TooBig:   out << "TooBig"s;   break;
        case Donut::Feedback::TooSmall: out << "TooSmall"s; break;
        case Donut::Feedback::Unknown:  out << "Unknown"s;  break;
    }
    return out;
}
