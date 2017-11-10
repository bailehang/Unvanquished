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
    auto winner_a = winner->client->pers.admin;
    auto loser_a = loser->client->pers.admin;
    bool elo_should_update = true;

    if (!winner_a) {
        elo_should_update = false;
        Log::Warn("winner %s has bogus admin pointer", winner->client->pers.netname);
    }
    if (!loser_a) {
        elo_should_update = false;
        Log::Warn("loser %s has bogus admin pointer", loser->client->pers.netname);
    }
    if (elo_should_update) {
        winner->client->pers.admin->elo = winner_score;
        loser->client->pers.admin->elo = loser_score;
    }
}
