
#include "face.hxx"

Face::Face (const Bv& b) : _basis{b}, _primes{vector<Bv>{}} {};

Face::Face (Face&& face2)
{
    _basis = move(face2._basis);
    _primes = move(face2._primes);
}

Face& Face::operator=(Face&& face2)
{
    _basis = move(face2._basis);
    _primes = move(face2._primes);
    return *this;
}

const Bv& Face::basis () const
{
    return _basis;
}

const vector<Bv>& Face::primes () const
{
    return _primes;
}

void Face::push (Bv&& b)
{
    assert(b);
    for (auto pit=_primes.begin(); pit!=_primes.end();)
        // absorb greater primes to avoid redundancy
        if ((b^basis()) < (*pit^basis()))
            _primes.erase(pit);
        else pit++;

    _primes.push_back(move(b));
}

bool Face::operator () (const Bv& b) const
{
    for (Bv p : primes())
        if ((p^basis()) < (b^basis()))
            return true;
    return false;
}

string Face::to_string () const
{
    string s = "{";
    s += basis() ? basis().to_string() : "-";

    s += " : ";
    if (primes().size() > 0)
    {
        for (const auto& bv : primes())
        {
            s += bv.to_string(); s += ",";
        }
        s.pop_back();
    }
    else
    {
        s += "-";
    }
    s += "}";

    return s;
}

string Face::to_string_pretty () const
{
    string s = "basis: ";
    s += basis() ? basis().to_string() : "null";

    s += "\nprimes: ";
    if (primes().size() > 0)
    {
        for (const auto& bv : primes())
        {
            s += bv.to_string(); s += ", ";
        }
        s.pop_back(); s.pop_back();
    }
    else
    {
        s += "null";
    }
    s += '\n';

    return s;
}
