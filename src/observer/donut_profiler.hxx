
#pragma once

#include <cassert>

#include <vector>
using std::vector;
#include <string>
using std::string;
using std::to_string;
using namespace std::string_literals;
#include <iostream>
using std::ostream;

#include <array>
using std::array;
#include <sstream>
using std::stringstream;
#include <iomanip>
using std::setw;
using std::setprecision;
using std::setfill;
#include <ios>
using std::fixed;
using std::left;
using std::right;

#include "stats.hxx"
#include "table.hxx"

namespace Donut
{
//=================================================================================================
// Object profiler
//
// Base profiler
constinit static const size_t prof_table_fields = 6;
using PTable = Table<prof_table_fields>;
using PRow   = PTable::Row;
struct Profiler
{
    virtual PTable get_table () const = 0;
    string to_string () const { return get_table().to_string(); }

    PRow fmt_header (const Stats& s) const;
    PRow fmt_line   (const Stats& nomi, const Stats& deno) const;
};
ostream& operator << (ostream& out, const Profiler& prof);

// Teacher profiler
struct TeacherProfiler : virtual public Profiler
{
    Stats teacher_total{"teacher_total"},
          // method time
          membership{"membership_time"},
          equivalence{"equivalence_time"},
          setup{"setup_time"},
          degen{"degen_time"},
          advanceable{"advanceable_time"},
          unroll{"unroll_time"},
          restart{"restart_time"},
          progressed{"progressed_time"},
          advance{"advance_time"},
          // sat time
          sat_total{"sat_total"},
          membership_sat{"membership_sat"},
          equivalence_progress_sat{"equivalence_progress_sat"},
          equivalence_soundness_sat{"equivalence_soundness_sat"},
          degen_sat{"degen_sat"},
          advanceable_sat{"advanceable_sat"},
          progressed_sat{"progressed_sat"};

    const vector<Stats> method_times () const
    {
        return vector<Stats>{
            membership,
            equivalence,
            setup,
            degen,
            advanceable,
            unroll,
            restart,
            progressed,
            advance
        };
    }

    const vector<Stats> sat_times () const
    {
        return vector<Stats>{
            membership_sat,
            equivalence_progress_sat,
            equivalence_soundness_sat,
            degen_sat,
            advanceable_sat,
            progressed_sat
        };
    }

    PTable get_table () const override;
};

// Learner profiler
struct LearnerProfiler : virtual public Profiler
{
    Stats learner_total{"learner_total"},
          // method time
          minimize{"minimize_time"},
          // algo time
          algo_total{"algo_total"},
          toobig{"toobig_time"},
          toosmall{"toosmall_time"};

    const vector<Stats> method_times () const
    {
        return vector<Stats>{
            minimize
        };
    }

    const vector<Stats> algo_times () const
    {
        return vector<Stats>{
            toobig,
            toosmall
        };
    }

    PTable get_table () const override;
};

// Context profiler aggregate
struct ContextProfiler : virtual public Profiler
{
    ContextProfiler (const TeacherProfiler& tp, const LearnerProfiler& lp) : tprof{tp}, lprof{lp} {}
    // teacher time
    const TeacherProfiler& tprof;
    const LearnerProfiler& lprof;

    PTable get_table () const override;
};

//=================================================================================================
// Stuff being profiled
//
struct Profiled
{
    virtual const Profiler& get_prof () const = 0;
};

//=================================================================================================
}
