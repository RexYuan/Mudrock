
#include "more_aig.hxx"

#include <iostream>
using std::cout;
using std::endl;

set<AigVar> findCOIof (const Aig& aig, const AigVar& source)
{
    const vector<AigVar>& inputs = aig.inputs();
    const vector<AigLatch>& latches = aig.latches();
    const vector<AigAnd>& ands = aig.ands();
    size_t input_offset = inputs[0],
           input_range  = inputs[inputs.size()-1]; // var range is contiguous
    size_t latches_offset = latches[0].curr,
           latches_range  = latches[latches.size()-1].curr; // var range is contiguous
    size_t and_offset = ands[0].var,
           and_range  = ands[ands.size()-1].var; // var range is contiguous

    set<AigVar> cone, seen;
    queue<AigVar> nodes;
    nodes.push(source);

    auto process_node = [&](AigVar n)
    {
        if (input_offset <= n && n <= input_range)
        {
            // input doesn't participate in extending frame
            // cout << "processed " << n << endl;
        }
        else if (latches_offset <= n && n <= latches_range)
        {
            cone.insert(n);
            AigVar v = latches[n - latches_offset].next.var;
            if (!seen.contains(v))
            {
                nodes.push(v);
                seen.insert(v);
            }
            // cout << "processed " << n << " <= " << v << endl;
        }
        else if (and_offset <= n && n <= and_range)
        {
            AigVar lv = ands[n - and_offset].l.var,
                   rv = ands[n - and_offset].r.var;
            if (!seen.contains(lv))
            {
                nodes.push(lv);
                seen.insert(lv);
            }
            if (!seen.contains(rv))
            {
                nodes.push(rv);
                seen.insert(rv);
            }
            // cout << "processed " << n << " => " << lv << " " << rv << endl;
        }
        else
        {
            assert(n == aig.outputs()[0].var || n == 0);
        }
    };

    while (!nodes.empty())
    {
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
