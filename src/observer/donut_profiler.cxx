
#include "donut_profiler.hxx"

namespace Donut
{
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
}
PRow Profiler::fmt_header (const Stats& s) const
{
    return PRow{s.title()+":", mk_time(s.cumu()), ""s, ""s, ""s, ""s};
}
PRow Profiler::fmt_line (const Stats& nomi, const Stats& deno) const
{
    return PRow{nomi.title() + ":",
                mk_time(nomi.cumu()),
                mk_percentage(nomi.cumu()/deno.cumu()),
                "#" + mk_count(nomi.runs()),
                mk_time(nomi.cumu()/nomi.runs()) + "(avg)"s,
                mk_time(fd_max(nomi.laps())) + "(max)"s};
}

// Teacher profiler
PTable TeacherProfiler::get_table () const
{
    PTable ret;

    ret.set_header(fmt_header(teacher_total));
    for (const Stats& method_time : method_times())
        ret.push_entry(fmt_line(method_time, teacher_total));

    ret.push_entry();

    ret.push_entry(fmt_header(sat_total));
    for (const Stats& sat_time : sat_times())
        ret.push_entry(fmt_line(sat_time, sat_total));

    return ret;
}

// Learner profiler
PTable LearnerProfiler::get_table () const
{
    PTable ret;

    ret.set_header(PRow{
        "name "s, "totime"s, "totime%"s, "#runs"s, "runtime(avg)"s, "runtime(max)"s
    });

    ret.push_entry(fmt_header(learner_total));
    for (const Stats& method_time : method_times())
        ret.push_entry(fmt_line(method_time, learner_total));

    ret.push_entry();

    ret.push_entry(fmt_header(algo_total));
    for (const Stats& algo_time : algo_times())
        ret.push_entry(fmt_line(algo_time, algo_total));

    return ret;
}

// Context profiler
PTable ContextProfiler::get_table () const
{
    PTable tmp;

    tmp.push_entry(lprof.get_table().header());
    for (auto row : lprof.get_table().entries())
        tmp.push_entry(row);

    tmp.push_entry();

    tmp.push_entry(tprof.get_table().header());
    for (auto row : tprof.get_table().entries())
        tmp.push_entry(row);

    return tmp;
}

ostream& operator << (ostream& out, const Profiler& prof)
{
    out << prof.to_string();
    return out;
}
//=================================================================================================
}
