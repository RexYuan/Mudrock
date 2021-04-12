
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
#include <concepts>
using std::same_as;

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

#include <chrono>
using sec = std::chrono::duration<double>;
using std::chrono::steady_clock;

namespace Donut
{
//=================================================================================================
// Basic profiling
//
enum class StatsState {Stopped, Running, Paused};
ostream& operator << (ostream& out, const StatsState& ss);
/*
 *          (start)    (pause)
 * -> Stopped -> Running -> Paused
 *          ^    |     ^    |
 *          |    |     |    |
 *          +----+     +----+
 *          (stop)     (resume)
 */
struct Stats;
template <typename T>     concept is_stats  = same_as<T, Stats>;
template <typename... Ts> concept are_stats = (is_stats<Ts> && ...);
struct Stats
{
    static const StatsState Stopped = StatsState::Stopped;
    static const StatsState Running = StatsState::Running;
    static const StatsState Paused  = StatsState::Paused;

    Stats (string t);

    void start  ();
    void stop   ();
    void resume ();
    void pause  ();

    inline const string& title () const { return _title; }
    inline const double  cumu  () const { return _cumu; }
    inline const size_t  runs  () const { return _runs; }
    inline const vector<double>& laps () const { return _laps; }

private:
    StatsState state{Stopped};

    string _title;
    double _cumu{0.0}; // cumulative time
    size_t _runs{0};   // number of laps
    vector<double> _laps; // splits

    steady_clock::time_point split;
    double tmp{0.0};
};

template <typename... Ts> requires are_stats<Ts...> inline void start  (Ts&... ss) { (ss.start(), ...); }
template <typename... Ts> requires are_stats<Ts...> inline void stop   (Ts&... ss) { (ss.stop(), ...); }
template <typename... Ts> requires are_stats<Ts...> inline void resume (Ts&... ss) { (ss.resume(), ...); }
template <typename... Ts> requires are_stats<Ts...> inline void pause  (Ts&... ss) { (ss.pause(), ...); }

//=================================================================================================
// Object profiler
//
// Base profiler
constinit const size_t table_fields = 6;
using trow = array<string, table_fields>;
using ttable = vector<trow>;
struct Profiler
{
    virtual ttable get_table () const = 0;
    static string print_stats (ttable table);
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

    ttable get_table () const override;
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

    ttable get_table () const override;
};

// Context profiler aggregate
struct ContextProfiler : virtual public Profiler
{
    ContextProfiler (const TeacherProfiler& tp, const LearnerProfiler& lp) : tprof{tp}, lprof{lp} {}
    // teacher time
    const TeacherProfiler& tprof;
    const LearnerProfiler& lprof;

    ttable get_table () const override;
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
