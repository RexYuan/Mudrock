
#include "B_types.hxx"

ostream& operator << (ostream& out, const B_Types::Feedback& feedback)
{
    switch (feedback)
    {
        case B_Types::Feedback::Refuted:  out << "Refuted"s;  break;
        case B_Types::Feedback::Perfect:  out << "Perfect"s;  break;
        case B_Types::Feedback::TooBig:   out << "TooBig"s;   break;
        case B_Types::Feedback::TooSmall: out << "TooSmall"s; break;
        case B_Types::Feedback::Unknown:  out << "Unknown"s;  break;
    }
    return out;
}
