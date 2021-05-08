
#pragma once

#include <cassert>

#include <string>
using std::string;
using namespace std::string_literals;
#include <iostream>
using std::cout;
using std::ostream;
using std::endl;
using std::flush;
#include <sstream>
using std::ostringstream;

#include <concepts>
using std::same_as;
using std::convertible_to;
#include <functional>
using std::function;

template <typename T>     concept is_convertible_to_string   = convertible_to<T,string>;
template <typename... Ts> concept are_convertible_to_strings = (is_convertible_to_string<Ts> && ...);
//=================================================================================================
// Basic logging
//
struct Logger
{
    Logger (ostream& o = cout);
    ~Logger ();

    template <typename... Ts> requires are_convertible_to_strings<Ts...>
    void log (const Ts&... msgs);

    void try_write ();
    void write (); // write `buffer` content to `outs`

private:
    ostream& outs; // actual stream to write to
    ostringstream buffer; // internal stream buffer
};

struct SingletonLogger
{
    static Logger& Get ()
    {
        static Logger logger{};
        return logger;
    }

    SingletonLogger () {}
    SingletonLogger           (const SingletonLogger&) = delete;
    SingletonLogger& operator=(const SingletonLogger&) = delete;
    SingletonLogger           (SingletonLogger&&) = delete;
    SingletonLogger& operator=(SingletonLogger&&) = delete;
};

template <typename... Ts> requires are_convertible_to_strings<Ts...>
void Logger::log (const Ts&... msgs)
{
#ifdef LOGGING
    (buffer << ... << msgs) << endl;
    try_write();
#endif
}

template <typename T, typename... Ts> requires are_convertible_to_strings<T, Ts...>
void log (size_t verbosity, const T& title, const Ts&... msgs)
{
#ifdef LOGGING
    SingletonLogger::Get().log("("s, title, ") "s, string(verbosity, '#'), " "s, msgs...);
#endif
}
