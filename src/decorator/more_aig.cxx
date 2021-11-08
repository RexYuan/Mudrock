
#include "more_aig.hxx"

#include <iostream>
using std::cout;
using std::endl;

set<AigVar> findCOIof (const Aig& aig, const AigVar& source)
{
    const vector<AigAnd>& ands = aig.ands();
    size_t and_offset = ands[0].var,
           and_range  = ands[ands.size()-1].var; // and var range is contiguous

    set<AigVar> cone, seen;
    queue<AigVar> nodes;
    nodes.push(source);

    auto process_node = [&](AigVar n)
    {
        AigVar lv = ands[n - and_offset].l.var,
               rv = ands[n - and_offset].r.var;

        if (and_offset <= lv && lv <= and_range)
        {
            if (!seen.contains(lv))
                nodes.push(lv);
        }
        else
            cone.insert(lv);

        if (and_offset <= rv && rv <= and_range)
        {
            if (!seen.contains(rv))
                nodes.push(rv);
        }
        else
            cone.insert(rv);

        seen.insert(lv);
        seen.insert(rv);
    };

    while (!nodes.empty())
    {
        // cout << "processing " << nodes.front()
        //      << " => " << ands[nodes.front() - and_offset].l.var
        //      << " " << ands[nodes.front() - and_offset].r.var << endl;
        process_node(nodes.front());
        nodes.pop();
    }

    // cout << endl;
    // for (auto k : cone)
    // {
    //     cout << k << endl;
    // }

    return cone;
}
