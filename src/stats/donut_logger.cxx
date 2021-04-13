
#include "donut_logger.hxx"

namespace Donut
{
//=================================================================================================
// Basic logging
//
Logger::Logger (ostream& o) :
outs{o} {}

Logger::~Logger ()
{
    write();
}

void Logger::try_write ()
{
    const  size_t buffer_limit = 1000;
    static size_t buffer_size  = 0;
    // only flush to outs every `buffer_limit` inputs
    if (buffer_size >= buffer_limit)
    {
        write();
        buffer_size = 0;
    }
    else
    {
        buffer_size++;
    }
}

void Logger::write ()
{
    outs << buffer.str() << flush;
    buffer.str(""s);
    buffer.clear();
}

//=================================================================================================
}
