
#include "stats.hxx"

ostream& operator << (ostream& out, const StatsState& ss)
{
    switch (ss)
    {
        case StatsState::Stopped: out << "Stopped"s; break;
        case StatsState::Running: out << "Running"s; break;
        case StatsState::Paused:  out << "Paused"s;  break;
    }
    return out;
}

//=================================================================================================
// Basic stats
//
Stats::Stats (string t) : _title{t} {}

void Stats::start ()
{
    assert(state == Stopped);

    split = steady_clock::now();

    state = Running;
}

void Stats::stop ()
{
    assert(state == Running);

    sec elapsed = steady_clock::now() - split;
    tmp += elapsed.count();
    _cumu += tmp;
    _laps.push_back(tmp);
    tmp = 0.0; // reset temp accumulator

    _runs++;

    state = Stopped;
}

void Stats::resume ()
{
    assert(state == Paused);

    split = steady_clock::now();

    state = Running;
}

void Stats::pause ()
{
    assert(state == Running);

    sec elapsed = steady_clock::now() - split;
    tmp += elapsed.count();

    state = Paused;
}
