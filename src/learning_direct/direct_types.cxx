
#include "direct_types.hxx"

ostream& operator << (ostream& out, const Direct::Feedback& feedback)
{
    switch (feedback)
    {
        case Direct::Feedback::Refuted:  out << "Refuted"s;  break;
        case Direct::Feedback::Perfect:  out << "Perfect"s;  break;
        case Direct::Feedback::TooBig:   out << "TooBig"s;   break;
        case Direct::Feedback::TooSmall: out << "TooSmall"s; break;
        case Direct::Feedback::Unknown:  out << "Unknown"s;  break;
    }
    return out;
}
