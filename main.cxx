
#include <cassert>
#include <cstdlib>

#include <string>
using namespace std::string_literals;
using std::string;
#include <sstream>
using std::stringstream;
#include <iostream>
using std::cout;
#include <ostream>
using std::endl;

#include "sig.hxx"

#include "direct_context.hxx"
#include "donut_context.hxx"

void print_usage ()
{
    cout << "usage: ./main.o d|m input.aag\n"s;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    string mode{argv[1]}, filename{argv[2]};
    stringstream output;

    if (mode == "d"s || mode == "D"s)
    {
        using namespace Direct;
        Context c{filename};
        c.check();
        output << (c.sat() ? "sat"s : "unsat"s);
    }
    else if (mode == "m"s || mode == "M"s)
    {
        signal(SIGTERM, sigterm_handler);

        using namespace Donut;
        Context c{filename};
        c.check();

#ifdef PROFILING
        SingletonLogger::Get().write();
        output << SingletonProfiler::Get();
#endif

        output << c.sat();
    }
    else
    {
        print_usage();
        return EXIT_FAILURE;
    }

    cout << output.str() << endl;

    return EXIT_SUCCESS;
}
