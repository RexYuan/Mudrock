
#include "m_types.hxx"

ostream& operator << (ostream& out, const M_Types::Feedback& feedback)
{
    switch (feedback)
    {
        case M_Types::Feedback::Refuted:  out << "Refuted"s;  break;
        case M_Types::Feedback::Perfect:  out << "Perfect"s;  break;
        case M_Types::Feedback::TooBig:   out << "TooBig"s;   break;
        case M_Types::Feedback::TooSmall: out << "TooSmall"s; break;
        case M_Types::Feedback::Unknown:  out << "Unknown"s;  break;
    }
    return out;
}
