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

int elo_get_loadout_value( gentity_t *ply )
{
    int score;
    switch ( ply->client->ps.weapon )
    {
		case WP_ALEVEL1:
            // basi
            score = 200;
			break;

		case WP_ALEVEL3:
            // goon
            score = 800;
			break;

		case WP_ALEVEL3_UPG:
            // +goon
            score = 1000;
			break;

		case WP_ALEVEL2:
            // mara
            score = 400;
			break;

		case WP_ALEVEL2_UPG:
            // +mara
            score = 600;
			break;

		case WP_ALEVEL4:
            // rant
            score = 1000;
			break;

		case WP_BLASTER:
            score = -50;
			break;

		case WP_MACHINEGUN:
            score = 0;
			break;

		case WP_SHOTGUN:
            score = 150;
			break;

		case WP_CHAINGUN:
            score = 400;
			break;

		case WP_FLAMER:
            score = 450;
			break;

		case WP_PULSE_RIFLE:
            score = 500;
			break;

		case WP_MASS_DRIVER:
            score = 400;
			break;

		case WP_LUCIFER_CANNON:
            score = 1000;
			break;

		case WP_LAS_GUN:
            score = 200;
			break;

		case WP_PAIN_SAW:
            score = 200;
			break;

		case WP_LOCKBLOB_LAUNCHER:
            score = 0;
			break;

		case WP_HIVE:
            score = 0;
			break;

		case WP_ROCKETPOD:
            score = 0;
			break;

		case WP_MGTURRET:
            score = 0;
			break;

		case WP_ABUILD:
		case WP_ABUILD2:
            score = 0;
			break;

		case WP_HBUILD:
            score = 0;
			break;

		default:
            Log::Warn("unrecognized weapon for %s", ply->client->pers.netname);
            score = 0;
			break;
    }
    return score;
}

void elo_calculate( gentity_t *winner, gentity_t *loser )
{
    if ( winner->client->pers.isFillerBot || loser->client->pers.isFillerBot ) { return; } // ignore bots

    int winner_elo = elo_get(winner->client->pers.admin);
    int loser_elo = elo_get(loser->client->pers.admin);

    int winner_transformed_elo = 10 ^ (winner_elo / 400);
    int loser_transformed_elo = 10 ^ (loser_elo / 400);

    int winner_expected = winner_transformed_elo / (winner_transformed_elo + loser_transformed_elo);
    int loser_expected = loser_transformed_elo / (winner_transformed_elo + loser_transformed_elo);

    // reduce kfactor if player loadouts are unbalanced
    // a tyrant killing a nude human shouldn't have the same elo bonus
    // as a dretch killing a chainsuit
    int effective_kfactor = elo_kfactor.integer;
    int winner_loadout_score = elo_get_loadout_value(winner);
    int loser_loadout_score = elo_get_loadout_value(loser);
    effective_kfactor = elo_kfactor.integer - ( ( winner_loadout_score - loser_loadout_score ) / elo_kfactor.integer );

    int winner_score = winner_elo + effective_kfactor * (1 - winner_expected);
    int loser_score = loser_elo + effective_kfactor * (0 - loser_expected);

    // commit elos to admin data
    auto winner_a = winner->client->pers.admin;
    auto loser_a = loser->client->pers.admin;
    bool elo_should_update = true;

    if (!winner_a) {
        elo_should_update = false;
        Log::Debug("winner %s has bogus admin pointer", winner->client->pers.netname);
    }
    if (!loser_a) {
        elo_should_update = false;
        Log::Debug("loser %s has bogus admin pointer", loser->client->pers.netname);
    }
    if (elo_should_update) {
        winner->client->pers.admin->elo = winner_score;
        loser->client->pers.admin->elo = loser_score;
    }
}
