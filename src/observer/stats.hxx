
#pragma once

#include <cassert>

#include <vector>
using std::vector;
#include <string>
using std::string;
using namespace std::string_literals;
#include <iostream>
using std::ostream;
#include <concepts>
using std::same_as;

#include <chrono>
using sec = std::chrono::duration<double>;
using std::chrono::steady_clock;

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

    const string _title;
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
