
#include "donut_profiler.hxx"

namespace Donut
{
ostream& operator << (ostream& out, const StatsState& ss)
{
    switch (ss)
    {
        case Donut::StatsState::Stopped: out << "Stopped"s; break;
        case Donut::StatsState::Running: out << "Running"s; break;
        case Donut::StatsState::Paused:  out << "Paused"s;  break;
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

//=================================================================================================
// Object profiler
//
namespace
{
    inline string trunc_trail (double num, size_t len)
    {
        stringstream tmp;
        tmp << fixed << setfill('0') << setprecision(len) << num;
        return tmp.str();
    }

    inline string mk_percentage (double num)
    {
        stringstream tmp;
        tmp << trunc_trail(num*100, 2) << "%"s;
        return tmp.str();
    }

    inline string mk_time (double num)
    {
        stringstream tmp;
        tmp << trunc_trail(num, 4);
        return tmp.str();
    }

    inline string mk_count (size_t num)
    {
        stringstream tmp;
        tmp << num;
        return tmp.str();
    }

    inline double fd_max (const vector<double>& nums)
    {
        double tmp = 0.0;
        for (auto num : nums)
            if (num > tmp)
                tmp = num;
        return tmp;
    }

    inline trow fmt_hder (const Stats& hd)
    {
        trow row{hd.title()+":", mk_time(hd.cumu()), ""s, ""s, ""s, ""s};
        return row;
    }

    inline trow fmt_line (const Stats& nomi, const Stats& deno)
    {
        trow row{nomi.title() + ":",
                 mk_time(nomi.cumu()),
                 mk_percentage(nomi.cumu()/deno.cumu()),
                 "#" + mk_count(nomi.runs()),
                 mk_time(nomi.cumu()/nomi.runs()) + "(avg)"s,
                 mk_time(fd_max(nomi.laps())) + " (max)"s};
        return row;
    }

    inline trow fmt_empty ()
    {
        trow row{""s, ""s, ""s, ""s, ""s, ""s};
        return row;
    }
}
// Teacher profiler
ttable TeacherProfiler::get_table () const
{
    ttable ret;

    ret.push_back(fmt_hder(teacher_total));
    for (const Stats& method_time : method_times())
        ret.push_back(fmt_line(method_time, teacher_total));

    ret.push_back(fmt_empty());

    ret.push_back(fmt_hder(sat_total));
    for (const Stats& sat_time : sat_times())
        ret.push_back(fmt_line(sat_time, sat_total));

    return ret;
}

// Learner profiler
ttable LearnerProfiler::get_table () const
{
    ttable ret;

    ret.push_back(fmt_hder(learner_total));
    for (const Stats& learn_time : learn_times())
        ret.push_back(fmt_line(learn_time, learner_total));

    return ret;
}

// Context profiler
ttable ContextProfiler::get_table () const
{
    ttable tmp;

    for (auto row : lprof.get_table())
        tmp.push_back(row);

    tmp.push_back(fmt_empty());

    for (auto row : tprof.get_table())
        tmp.push_back(row);

    return tmp;
}

namespace
{
    array<size_t, table_fields> maxlens (const vector<array<string, table_fields>>& table)
    {
        size_t title_wd{}, cumu_wd{}, perc_wd{}, runs_wd{}, avg_wd{}, max_wd{};
        for (auto [row_title, row_cumu, row_perc, row_runs, row_avg, row_max] : table)
        {
            if (row_title.length() > title_wd) title_wd = row_title.length();
            if (row_cumu.length()  > cumu_wd)  cumu_wd  = row_cumu.length();
            if (row_perc.length()  > perc_wd)  perc_wd  = row_perc.length();
            if (row_runs.length()  > runs_wd)  runs_wd  = row_runs.length();
            if (row_avg.length()   > avg_wd)   avg_wd   = row_avg.length();
            if (row_max.length()   > max_wd)   max_wd   = row_max.length();
        }
        return array<size_t, table_fields>{title_wd, cumu_wd, perc_wd, runs_wd, avg_wd, max_wd};
    }
}
// Base profiler
string Profiler::print_stats (ttable table)
{
    auto [title_wd, cumu_wd, perc_wd, runs_wd, avg_wd, max_wd] = maxlens(table);
    auto print_line = [=](trow row) -> string
    {
        auto [title, cumu, perc, runs, avg, max] = row;
        stringstream tmp;
        tmp << right
            << setw(title_wd) << title << " "
            << setw(cumu_wd)  << cumu  << " "
            << setw(perc_wd)  << perc  << " "
            << left
            << setw(runs_wd)  << runs  << " "
            << right
            << setw(avg_wd)   << avg   << " "
            << setw(max_wd)   << max   << "\n";
        return tmp.str();
    };

    string ret = ""s;
    for (trow row : table)
        ret += print_line(row);

    return ret;
}

ostream& operator << (ostream& out, const Profiler& prof)
{
    out << Profiler::print_stats(prof.get_table());
    return out;
}
//=================================================================================================
}
