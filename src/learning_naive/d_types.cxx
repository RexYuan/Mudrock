
#include "d_types.hxx"

ostream& operator << (ostream& out, const D_Types::Feedback& feedback)
{
    switch (feedback)
    {
        case D_Types::Feedback::Refuted:  out << "Refuted"s;  break;
        case D_Types::Feedback::Perfect:  out << "Perfect"s;  break;
        case D_Types::Feedback::TooBig:   out << "TooBig"s;   break;
        case D_Types::Feedback::TooSmall: out << "TooSmall"s; break;
        case D_Types::Feedback::Unknown:  out << "Unknown"s;  break;
    }
    return out;
}
