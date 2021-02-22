
#pragma once

#include <cassert>

#include <vector>
#include <string>

#include <fstream>
#include <utility>

using std::string;
using std::vector;
using std::ifstream;
using std::move;

//=================================================================================================
// And-Inverter Graphs
//
using AigVar = unsigned;
struct AigLit;
struct AigLatch;
struct AigAnd;
struct Aig
{
    Aig () = default;
    Aig (const string& filename);
    ~Aig () = default;
    
    Aig (const Aig& aig2) = delete; // disallow copy
    Aig& operator=(const Aig& aig2) = delete; // disallow copy
    
    Aig (Aig&& aig2); // use move
    Aig& operator=(Aig&& aig2); // use move

    // Getters
    //
    const string title            () const;
    const size_t maxvar           () const;
    const size_t num_inputs       () const;
    const size_t num_latches      () const;
    const size_t num_outputs      () const;
    const size_t num_ands         () const;

    const vector<AigVar>&   inputs  () const;
    const vector<AigLatch>& latches () const;
    const vector<AigLit>&   outputs () const;
    const vector<AigAnd>&   ands    () const;

    // Read inputs
    //
    void readAiger (string filename); // NOTE: ASCII mode only for now

    explicit operator bool () const; // check if aig is empty

private:
    string _title;
    size_t _maxvar;
    size_t _num_inputs;
    size_t _num_latches;
    size_t _num_outputs;
    size_t _num_ands;

    vector<AigVar>   _inputs;
    vector<AigLatch> _latches;
    vector<AigLit>   _outputs;
    vector<AigAnd>   _ands;

    ifstream fAiger;
    void readAigerHeaderLine ();
    void readAigerInputLine  ();
    void readAigerLatchLine  ();
    void readAigerOutputLine ();
    void readAigerAndLine    ();
};

//=================================================================================================
// Circuit Nodes
//
struct AigLit // TODO: use lifted literals
{
    bool sign;
    AigVar var;
};
inline constinit AigLit AigTrue  { true, 0};
inline constinit AigLit AigFalse {false, 0};

struct AigLatch
{
    AigVar curr;
    AigLit next;
};

struct AigAnd
{
    AigVar var;
    AigLit l,r;
};
