#include "app/certs.h"

#include <czmq.h>
#include <zcert.h>

namespace Clipd::App
{
void GenerateCertificate( fs::path cert )
{
    zcert_t* certificate = zcert_new();
    fs::path public_key = cert;
    public_key += ".pub";

    zcert_set_meta( certificate, "name", "Clipd" );
    zcert_set_meta( certificate, "url", "https://github.com/Notgnoshi/clipd" );
    zcert_set_meta( certificate, "email", "Austin.Gill@mines.sdsmt.edu" );

    zcert_save_secret( certificate, cert.string().c_str() );
    zcert_save_public( certificate, public_key.string().c_str() );

    zcert_destroy( &certificate );
}
} // namespace Clipd::App
