
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
#include <map>
using std::map;

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

//=================================================================================================
// Stats profiler
//
constinit static const size_t prof_table_fields = 6;
using PTable = Table<prof_table_fields>;
using PRow   = PTable::Row;
static const PRow fields_header{"name"s, "totime"s, "totime%"s, "#runs"s,
                                "runtime(avg)"s, "runtime(max)"s};
struct Profiler
{
    map<string, Stats>& get_stats_set (string set_name);
    Stats&              get_stats     (string set_name, string name);

    string to_string () const;

private:
    map<string, map<string, Stats>> stats_sets;

    PRow fmt_header (const string& title, double cum) const;
    PRow fmt_line   (const Stats& nomi,   double cum) const;

    PTable get_table () const;
};
ostream& operator << (ostream& out, const Profiler& prof);

struct SingletonProfiler
{
    static Profiler& Get ()
    {
        static Profiler prof{};
        return prof;
    }

    static Stats& GetStats (string set_name, string name)
    {
        return SingletonProfiler::Get().get_stats(set_name, name);
    }

    static void Start  (string set_name, string name) { start (SingletonProfiler::GetStats(set_name, name)); }
    static void Stop   (string set_name, string name) { stop  (SingletonProfiler::GetStats(set_name, name)); }
    static void Resume (string set_name, string name) { resume(SingletonProfiler::GetStats(set_name, name)); }
    static void Pause  (string set_name, string name) { pause (SingletonProfiler::GetStats(set_name, name)); }

    SingletonProfiler () {}
    SingletonProfiler           (const SingletonProfiler&) = delete;
    SingletonProfiler& operator=(const SingletonProfiler&) = delete;
    SingletonProfiler           (SingletonProfiler&&) = delete;
    SingletonProfiler& operator=(SingletonProfiler&&) = delete;
};

#define PROF_SCOPE_NAME_HELPER(line, count)                                   \
    __plz_no_collision_ ## line ## _ ## count

#define PROF_SCOPE_NAME(line, count)                                          \
    PROF_SCOPE_NAME_HELPER(line, count)

#define PROF_SCOPE_AS(verbosity, set_name, name)                              \
    struct ScopeProfiler                                                      \
    {                                                                         \
        ScopeProfiler (string sn_, string n_) : sn{sn_}, n{n_}                \
            { SingletonProfiler::Start(sn, n);                                \
              log(verbosity, sn, "Began ", n); }                              \
       ~ScopeProfiler ()                                                      \
            { SingletonProfiler::Stop (sn, n);                                \
              log(verbosity, sn, "Ended ", n,                                 \
                  ", time spent: ", to_string(                                \
                  SingletonProfiler::GetStats(sn, n).laps().back())); }       \
    private:                                                                  \
        string sn, n;                                                         \
    } PROF_SCOPE_NAME(__LINE__, __COUNTER__) {set_name, name}

#define PROF_SCOPE()                                                          \
    PROF_SCOPE_AS(1, __FILE__, __func__)

#define ID_NAME(name, id)                                                     \
    (string(id) == ""s ? string(name) : string(name) + " "s + string(id))

#define PROF_SCOPE_WITH_ID(sid, nid)                                          \
    PROF_SCOPE_AS(2, ID_NAME(__FILE__, sid), ID_NAME(__func__, nid))
