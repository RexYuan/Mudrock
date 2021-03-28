
#include <cassert>
#include <cstdlib>

#include <iostream>
using std::cout;

#include "d_context.hxx"
#include "m_context.hxx"
#include "b_context.hxx"

void print_usage ()
{
    cout << "usage: ./main.o d|m|b input.aag\n";
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        print_usage();
        return EXIT_FAILURE;
    }

    string mode{argv[1]}, filename{argv[2]};
    if (mode == "d" || mode == "D")
    {
        D_Context c{filename};
        c.check();
        if (c.sat())
            cout << "sat";
        else
            cout << "unsat";
    }
    else if (mode == "m" || mode == "M")
    {
        M_Context c{filename};
        c.check();
        if (c.sat())
            cout << "sat";
        else
            cout << "unsat";
    }
    else if (mode == "b" || mode == "B")
    {
        B_Context c{filename};
        c.check();
        if (c.sat())
            cout << "sat";
        else
            cout << "unsat";
    }
    else
    {
        print_usage();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
