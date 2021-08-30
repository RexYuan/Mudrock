
#include "face.hxx"

Face::Face (Bv* b) : _basis{b}, _primes{vector<Bv*>{}} {};

Face::Face (Face&& face2)
{
    _basis = face2._basis;
    _primes = move(face2._primes);
}

Face& Face::operator=(Face&& face2)
{
    _basis = face2._basis;
    _primes = move(face2._primes);
    return *this;
}

Bv* Face::basis () const
{
    return _basis;
}

const vector<Bv*>& Face::primes () const
{
    return _primes;
}

void Face::push (Bv* b)
{
    assert(b);
    if ((*this)(b)) return;

    for (auto pit=_primes.begin(); pit!=_primes.end();)
        // absorb greater primes to avoid redundancy
        if ((*b^*basis()) < (**pit^*basis()))
            _primes.erase(pit);
        else pit++;

    _primes.push_back(b);
}

bool Face::size () const
{
    return primes().size();
}

bool Face::empty () const
{
    return primes().empty();
}

bool Face::operator () (Bv* b) const
{
    for (Bv* p : primes())
        if ((*p^*basis()) < (*b^*basis()))
            return true;
    return false;
}

string Face::to_string () const
{
    string s = "{";
    s += basis() ? basis()->to_string() : "-";

    s += " : ";
    if (primes().size() > 0)
    {
        for (auto bv : primes())
        {
            s += bv->to_string(); s += ",";
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
    s += basis() ? basis()->to_string() : "null";

    s += "\nprimes: ";
    if (primes().size() > 0)
    {
        for (auto bv : primes())
        {
            s += bv->to_string(); s += ", ";
        }
        s.pop_back(); s.pop_back();
    }
    else
    {
        s += "null";
    }
    s += "\n";

    return s;
}
