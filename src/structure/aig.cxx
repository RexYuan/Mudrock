
#include "aig.hxx"

//=================================================================================================
// And-Inverter Graphs
//
Aig::Aig (const string& filename)
{
    readAiger (filename);
}

Aig::Aig (Aig&& aig2)
{
    _title       = move(aig2._title);
    _maxvar      = aig2._maxvar;
    _num_inputs  = aig2._num_inputs;
    _num_latches = aig2._num_latches;
    _num_outputs = aig2._num_outputs;
    _num_ands    = aig2._num_ands;
    _inputs      = move(aig2._inputs);
    _latches     = move(aig2._latches);
    _outputs     = move(aig2._outputs);
    _ands        = move(aig2._ands);

    aig2._title       = "";
    aig2._maxvar      = 0;
    aig2._num_inputs  = 0;
    aig2._num_latches = 0;
    aig2._num_outputs = 0;
    aig2._num_ands    = 0;
    // vector move constructor guarantees empty()
}

Aig& Aig::operator=(Aig&& aig2)
{
    _title       = move(aig2._title);
    _maxvar      = aig2._maxvar;
    _num_inputs  = aig2._num_inputs;
    _num_latches = aig2._num_latches;
    _num_outputs = aig2._num_outputs;
    _num_ands    = aig2._num_ands;
    _inputs      = move(aig2._inputs);
    _latches     = move(aig2._latches);
    _outputs     = move(aig2._outputs);
    _ands        = move(aig2._ands);

    aig2._title       = "";
    aig2._maxvar      = 0;
    aig2._num_inputs  = 0;
    aig2._num_latches = 0;
    aig2._num_outputs = 0;
    aig2._num_ands    = 0;
    aig2._inputs      .clear();
    aig2._latches     .clear();
    aig2._outputs     .clear();
    aig2._ands        .clear();

    return *this;
}

// Getters
//
const string Aig::title       () const { return _title; }
const size_t Aig::maxvar      () const { return _maxvar; }
const size_t Aig::num_inputs  () const { return _num_inputs; }
const size_t Aig::num_latches () const { return _num_latches; }
const size_t Aig::num_outputs () const { return _num_outputs; }
const size_t Aig::num_ands    () const { return _num_ands; }

const vector<AigVar>&   Aig::inputs  () const { return _inputs; }
const vector<AigLatch>& Aig::latches () const { return _latches; }
const vector<AigLit>&   Aig::outputs () const { return _outputs; }
const vector<AigAnd>&   Aig::ands    () const { return _ands; }

// Aiger file raw lit operations
//
namespace
{
    inline constexpr bool   get_sign   (unsigned raw) { return (raw&1)==0; }
    inline constexpr AigVar get_var    (unsigned raw) { return raw>>1; }
    inline constexpr bool   is_const_t (unsigned raw) { return raw==1; }
    inline constexpr bool   is_const_f (unsigned raw) { return raw==0; }
}

// Read inputs
//
void Aig::readAiger (string filename)
{
    assert(!filename.empty());
    fAiger.open(filename, std::ios::in);
    assert(fAiger.is_open());

    readAigerHeaderLine();

    for (size_t i=0; i<num_inputs(); i++)
        readAigerInputLine();
    
    for (size_t i=0; i<num_latches(); i++)
        readAigerLatchLine();
    
    for (size_t i=0; i<num_outputs(); i++)
        readAigerOutputLine();

    for (size_t i=0; i<num_ands(); i++)
        readAigerAndLine();

    fAiger.close();
}

void Aig::readAigerHeaderLine ()
{
    assert(fAiger.is_open());
    fAiger >> _title >> _maxvar >> _num_inputs >> _num_latches >> _num_outputs >> _num_ands;
    assert(_maxvar == _num_inputs + _num_latches + _num_ands);
}

void Aig::readAigerInputLine ()
{
    assert(fAiger.is_open());

    unsigned input;
    fAiger >> input;
    assert(get_var(input) <= _maxvar);
    assert(!is_const_t(input) && !is_const_f(input));

    _inputs.emplace_back(get_var(input));
}

void Aig::readAigerLatchLine ()
{
    assert(fAiger.is_open());

    unsigned curr, next;
    fAiger >> curr >> next;
    assert(get_var(curr) <= _maxvar);
    assert(get_var(next) <= _maxvar);
    assert(!is_const_t(curr) && !is_const_f(curr));

    _latches.emplace_back(get_var(curr), AigLit{get_sign(next), get_var(next)});
}

void Aig::readAigerOutputLine ()
{
    assert(fAiger.is_open());

    unsigned output;
    fAiger >> output;
    assert(get_var(output) <= _maxvar);

    if (is_const_t(output))
        _outputs.push_back(AigTrue);
    else if (is_const_f(output))
        _outputs.push_back(AigFalse);
    else
        _outputs.emplace_back(get_sign(output), get_var(output));
}

void Aig::readAigerAndLine ()
{
    assert(fAiger.is_open());

    unsigned parent, left_child, right_child;
    fAiger >> parent >> left_child >> right_child;
    assert(get_var(parent) <= _maxvar);
    assert(get_var(left_child) <= _maxvar);
    assert(get_var(right_child) <= _maxvar);
    assert(!is_const_t(parent) && !is_const_f(parent));
    assert(parent > left_child && parent > right_child); // assumed ordered

    AigLit left_lit  = is_const_t(left_child) ? AigTrue  :
                       is_const_f(left_child) ? AigFalse :
                       AigLit{get_sign(left_child), get_var(left_child)};
    AigLit right_lit = is_const_t(right_child) ? AigTrue  :
                       is_const_f(right_child) ? AigFalse :
                       AigLit{get_sign(right_child), get_var(right_child)};

    _ands.emplace_back(get_var(parent), left_lit, right_lit);
}

Aig::operator bool () const
{
    return inputs() .size() +
           latches().size() +
           outputs().size() +
           ands()   .size() > 0;
}
