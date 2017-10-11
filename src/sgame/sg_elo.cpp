#include "sg_local.h"
#include "sg_elo.h"
#include "engine/qcommon/q_unicode.h"

int elo_get( g_admin_admin_t *a )
{
    if ( !a )
    {
        Log::Warn("elo_get called on bogus admin pointer");
        return elo_startvalue.integer;
    }

    if ( !a->elo )
    {
        a->elo = elo_startvalue.integer;
    }
    return a->elo;
}

