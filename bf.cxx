
#include "bf.hxx"

//=================================================================================================
// Pointer manipulation helpers
//
Bf_ptr v (bool b)
{
    return make_shared<Bf>(b);
}

Bf_ptr v (int i)
{
    return make_shared<Bf>(i);
}

Bf_ptr neg (Bf_ptr bf)
{
    switch (bf->t)
    {
    case Conn::Top: return v(false);
    case Conn::Bot: return v(true);
    case Conn::Not: return bf->get_sub();
    default: return make_shared<Bf>(Conn::Not, bf);
    }
}

Bf_ptr conj(Bf_ptr bf1, Bf_ptr bf2)
{
    if (bf1->t == Conn::Bot ||
        bf2->t == Conn::Bot) return v(false);
    else if (bf1->t == Conn::Top) return bf2;
    else if (bf2->t == Conn::Top) return bf1;
    else if (bf1->t == Conn::And &&
             bf2->t == Conn::And)
    {
        for (auto s : bf2->get_subs())
            bf1->push_sub(s);
        return bf1;
    }
    else if (bf1->t == Conn::And)
    {
        bf1->push_sub(bf2);
        return bf1;
    }
    else if (bf2->t == Conn::And)
    {
        bf2->push_sub(bf1);
        return bf2;
    }
    else
    {
        return make_shared<Bf>(Conn::And, bf1, bf2);
    }
}

Bf_ptr disj(Bf_ptr bf1, Bf_ptr bf2)
{
    if (bf1->t == Conn::Top ||
        bf2->t == Conn::Top) return v(true);
    else if (bf1->t == Conn::Bot) return bf2;
    else if (bf2->t == Conn::Bot) return bf1;
    else if (bf1->t == Conn::Or &&
             bf2->t == Conn::Or)
    {
        for (auto s : bf2->get_subs())
            bf1->push_sub(s);
        return bf1;
    }
    else if (bf1->t == Conn::Or)
    {
        bf1->push_sub(bf2);
        return bf1;
    }
    else if (bf2->t == Conn::Or)
    {
        bf2->push_sub(bf1);
        return bf2;
    }
    else
    {
        return make_shared<Bf>(Conn::Or, bf1, bf2);
    }
}

bool Bf::get_bool ()
{
    assert(t == Conn::Top || t == Conn::Bot);
    switch (t)
    {
        case Conn::Top: return true;
        case Conn::Bot: return false;
        default: break;
    }
    assert(false);
}

int Bf::get_int ()
{
    assert(t == Conn::Base);
    return get<int>(sub);
}

Bf_ptr Bf::get_sub ()
{
    assert(t == Conn::Not);
    return get<vector<Bf_ptr>>(sub)[0];
}

vector<Bf_ptr> Bf::get_subs ()
{
    assert(t == Conn::And || t == Conn::Or);
    return get<vector<Bf_ptr>>(sub);
}

void Bf::push_sub (Bf_ptr bf)
{
    assert(holds_alternative<vector<Bf_ptr>>(sub));
    get<vector<Bf_ptr>>(sub).push_back(bf);
}

string Bf::to_string ()
{
    switch (t)
    {
    case Conn::Top: return "t";
    case Conn::Bot: return "f";
    case Conn::Base: return std::to_string(get_int());
    case Conn::Not: return "~"+get_sub()->to_string();
    case Conn::And:
    {
        string tmp = "(";
        for (Bf_ptr s : get_subs())
            tmp.append( s->to_string() + "&" );
        tmp.pop_back();
        tmp.append(")");
        return tmp;
    }
    case Conn::Or:
    {
        string tmp = "(";
        for (Bf_ptr s : get_subs())
            tmp.append( s->to_string() + "|" );
        tmp.pop_back();
        tmp.append(")");
        return tmp;
    }
    }
    assert(false);
}
