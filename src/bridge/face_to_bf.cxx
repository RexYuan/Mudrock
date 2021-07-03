
#include "face_to_bf.hxx"

//=================================================================================================
// Face => Bf
//
struct IndexVars
{
    vector<Bf_ptr> vars, nvars;

    IndexVars (size_t s)
    {
        for (int i=0; i<static_cast<int>(s); i++)
        {
            vars. push_back(make_shared<Bf>(i));
            nvars.push_back(make_shared<Bf>(Conn::Not, vars[i]));
        }
    }
};

Bf_ptr toBf (const Face& face)
{
    static IndexVars cv{face.basis().len()}; // cache bv index vars

    Bf_ptr disj = v(false);

    Bf_ptr clus; int i;
    for (const Bv& term : face.primes())
    {
        clus = v(true);

        i = 0; // bf vars are mapped to bv index
        for (auto bit=face.basis().begin(), tit=term.begin();
             bit != face.basis().end(); ++bit, ++tit, ++i)
        {
            if (!*bit && *tit)
                clus = clus & cv.vars[i];
            else if (*bit && !*tit)
                clus = clus & cv.nvars[i];
        }

        disj = disj | clus;
    }
    return disj;
}
