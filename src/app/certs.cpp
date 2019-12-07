#include "app/certs.h"

namespace Clipd::App
{
void GenerateCertificate( fs::path path )
{
    zcert_t* certificate = zcert_new();

    zcert_set_meta( certificate, "name", "Clipd" );
    zcert_set_meta( certificate, "url", "https://github.com/Notgnoshi/clipd" );
    zcert_set_meta( certificate, "email", "Austin.Gill@mines.sdsmt.edu" );

    zcert_save( certificate, path.string().c_str() );
    zcert_destroy( &certificate );
}

zcert_t* LoadCertificate( fs::path cert )
{
    //! @note The caller is responsible for destroying the returned certificate.
    return zcert_load( cert.string().c_str() );
}
} // namespace Clipd::App
