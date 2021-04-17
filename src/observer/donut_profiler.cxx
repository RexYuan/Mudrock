
#include "donut_profiler.hxx"

//=================================================================================================
// Stats profiler
//
map<string, Stats>& Profiler::get_stats_set (string set_name)
{
    if (!stats_sets.contains(set_name))
        stats_sets.emplace(set_name, map<string, Stats>{});
    return stats_sets.at(set_name);
}

Stats& Profiler::get_stats (string set_name, string name)
{
    if (!get_stats_set(set_name).contains(name))
        get_stats_set(set_name).emplace(name, name);
    return get_stats_set(set_name).at(name);
}

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
}
PRow Profiler::fmt_header (const string& title, double cum) const
{
    return PRow{title, mk_time(cum), ""s, ""s, ""s, ""s};
}
PRow Profiler::fmt_line (const Stats& nomi, double cum) const
{
    return PRow{nomi.title(),
                mk_time(nomi.cumu()),
                mk_percentage(nomi.cumu()/cum),
                "#" + mk_count(nomi.runs()),
                mk_time(nomi.cumu()/nomi.runs()) + "(avg)"s,
                mk_time(fd_max(nomi.laps())) + "(max)"s};
}

namespace
{
    inline double sum_stats_set (map<string, Stats> stats_set)
    {
        double cum = 0.0;
        for (auto [stats_name, stats] : stats_set)
            cum = cum + stats.cumu();
        return cum;
    }
}
PTable Profiler::get_table () const
{
    PTable ret;

    ret.set_header(fields_header);

    for (auto [set_name, stats_set] : stats_sets)
    {
        double set_cum = sum_stats_set(stats_set);
        ret.push_entry(fmt_header(set_name, set_cum));
        for (auto [stats_name, stats] : stats_set)
        {
            ret.push_entry(fmt_line(stats, set_cum));
        }
        ret.push_entry();
    }

    return ret;
}

string Profiler::to_string () const
{
    return get_table().to_string();
}

ostream& operator << (ostream& out, const Profiler& prof)
{
    out << prof.to_string();
    return out;
}
