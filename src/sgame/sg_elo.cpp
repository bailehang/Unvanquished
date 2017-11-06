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

void elo_calculate( gentity_t *winner, gentity_t *loser ) {
    int winner_elo = elo_get(winner->client->pers.admin);
    int loser_elo = elo_get(loser->client->pers.admin);

    int winner_transformed_elo = 10 ^ (winner_elo / 400);
    int loser_transformed_elo = 10 ^ (loser_elo / 400);

    int winner_expected = winner_transformed_elo / (winner_transformed_elo + loser_transformed_elo);
    int loser_expected = loser_transformed_elo / (winner_transformed_elo + loser_transformed_elo);

    int winner_score = winner_elo + elo_kfactor.integer * (1 - winner_expected);
    int loser_score = loser_elo + elo_kfactor.integer * (0 - loser_expected);

    // commit elos to admin data
    winner->client->pers.admin->elo = winner_score;
    loser->client->pers.admin->elo = loser_score;
}
