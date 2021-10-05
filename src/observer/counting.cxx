
#include "counting.hxx"

//=================================================================================================
// Count clauses
//
size_t dryCountClause (const Bf_ptr& bf)
{
    switch (bf->get_type())
    {
    case Conn::Top:  [[fallthrough]];
    case Conn::Bot:  [[fallthrough]];
    case Conn::Base: return 0;
    case Conn::Not:  return 2 + dryCountClause(bf->get_sub());
    case Conn::And:  [[fallthrough]];
    case Conn::Or:
    {
        size_t tmp_sum = 0;
        for (Bf_ptr sub : bf->get_subs())
            tmp_sum += (1 + dryCountClause(sub));
        return 1 + tmp_sum;
    }
    }
    throw InvalidBfConn("Unmatched cases."s);
}

size_t dryCountClause (const Aig& aig)
{
    size_t sum = 0;
    for (const auto& [aigvar,aiglit1,aiglit2] : aig.ands())
    {
        Bf_ptr tmpbf = toBf(aiglit1) & toBf(aiglit2);
        sum += dryCountClause(tmpbf);
    }
    return sum;
}

