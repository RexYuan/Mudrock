
#include "face_to_bf.hxx"

//=================================================================================================
// Face => Bf
//
Bf_ptr toBf (const Face& face)
{
    Bf_ptr dj = disj();
    dj->reserve(face.primes().size());

    Bf_ptr clus; int i;
    for (const Bv_ptr term : face.primes())
    {
        clus = conj();
        clus->reserve(face.basis()->len());

        i = 0; // bf vars are mapped to bv index
        for (auto bit=face.basis()->begin(), tit=term->begin();
             bit != face.basis()->end(); ++bit, ++tit, ++i)
        {
            if (!*bit && *tit)
                clus += v(i);
            else if (*bit && !*tit)
                clus += ~v(i);
        }

        dj += clus;
    }
    return dj;
}
