
#include "face_to_bf.hxx"

//=================================================================================================
// Face => Bf
//
Bf_ptr toBf (const Face& face)
{
    Bf_ptr disj = v(false);

    Bf_ptr clus; int i;
    for (Bv* term : face.primes())
    {
        clus = v(true);

        i = 0; // bf vars are mapped to bv index
        for (auto bit=face.basis()->begin(), tit=term->begin();
             bit != face.basis()->end(); ++bit, ++tit, ++i)
        {
            if (!*bit && *tit)
                clus = clus & v(i);
            else if (*bit && !*tit)
                clus = clus & ~v(i);
        }

        disj = disj | clus;
    }
    return disj;
}
