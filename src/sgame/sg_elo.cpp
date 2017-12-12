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
    /*switch ( ply->client->ps.weapon )
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
    }*/
    if ( ply->client->pers.team == TEAM_HUMANS )
    {
        for ( int cur_weapon = WP_NONE + 1; cur_weapon < WP_NUM_WEAPONS; cur_weapon++ )
        {
            int weaponvalue;
            switch ( cur_weapon )
            {
	        	case WP_MACHINEGUN:
                    weaponvalue = 0;
	        		break;

	        	case WP_SHOTGUN:
                    weaponvalue = 150;
	        		break;

	        	case WP_CHAINGUN:
                    weaponvalue = 400;
	        		break;

	        	case WP_FLAMER:
                    weaponvalue = 450;
	        		break;

	        	case WP_PULSE_RIFLE:
                    weaponvalue = 500;
	        		break;

	        	case WP_MASS_DRIVER:
                    weaponvalue = 400;
	        		break;

	        	case WP_LUCIFER_CANNON:
                    weaponvalue = 1000;
	        		break;

	        	case WP_LAS_GUN:
                    weaponvalue = 200;
	        		break;

	        	case WP_PAIN_SAW:
                    weaponvalue = 200;
	        		break;

                default:
                    Log::Warn("human player %s^3 has an unrecognized weapon (%d)", ply->client->pers.netname, cur_weapon);
                    weaponvalue = 0;
            }
            if ( BG_InventoryContainsWeapon( cur_weapon, ply->client->ps.stats ) )
            {
                score = score + weaponvalue;
            }
        }

        for ( int cur_upgrade = UP_NONE + 1; cur_upgrade < UP_NUM_UPGRADES; cur_upgrade++ )
        {
            int upgradevalue;
            switch ( cur_upgrade )
            {
                case UP_LIGHTARMOUR:
                    upgradevalue = 150;
                    break;
                case UP_MEDIUMARMOUR:
                    upgradevalue = 250;
                    break;
                case UP_BATTLESUIT:
                    upgradevalue = 400;
                    break;
                case UP_RADAR:
                    upgradevalue = 100;
                    break;
                case UP_JETPACK:
                    upgradevalue = 150;
                    break;
                case UP_GRENADE:
                    upgradevalue = 200;
                    break;
                case UP_FIREBOMB:
                    upgradevalue = 200;
                    break;
                case UP_MEDKIT:
                    upgradevalue = 0; // don't count medkits
                    break;
                default:
                    Log::Warn("human player %s has an unrecognized upgrade (%d)", ply->client->pers.netname, cur_upgrade);
                    upgradevalue = 0;
            }
            if ( BG_InventoryContainsUpgrade( cur_upgrade, ply->client->ps.stats ) )
            {
                score = score + upgradevalue;
            }
        }
    }
    else if ( ply->client->pers.team == TEAM_ALIENS )
    {
        Log::Warn("FIXME: alien elo adjustments are not yet implemented");
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
    // FIXME: results in bizarre negative elos for some reason, need to debug
    int effective_kfactor = elo_kfactor.integer;
    /*
    int winner_loadout_score = elo_get_loadout_value(winner);
    int loser_loadout_score = elo_get_loadout_value(loser);
    effective_kfactor = elo_kfactor.integer - ( ( winner_loadout_score - loser_loadout_score ) / elo_kfactor.integer );
    Log::Debug("Effective K-factor for this kill is %d", effective_kfactor);
    */

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

void Cmd_elo_dbg_printinventoryvalue( gentity_t *ply )
{
    // get target name from argv[1]
    char tgtname[ MAX_TOKEN_CHARS ];
    trap_Argv( 1, tgtname, sizeof( tgtname ) ); 

    if ( !tgtname[0] )
    {
        // user did not supply args
        trap_SendServerCommand( ply - g_entities,
            va( "print_tr %s", QQ( N_("^3elo_dbg_printinventoryvalue: ^*this command requires a target name") ) ) // so lets bitch about it
        );
        return;
    }

    char err[ MAX_STRING_CHARS ];
    int tgtid = G_ClientNumberFromString( tgtname, err, sizeof( err ) );
    if ( tgtid == -1 )
    {
        G_admin_print( ply, va( "%s %s",
            QQ( N_( "^3elo_dbg_printinventoryvalue: ^*$1$" ) ),
            err
        ) );
        return;
    }

    // we have our target player
    gentity_t* target = &g_entities[tgtid];
    int loadout_value = elo_get_loadout_value( target );
    G_admin_print( ply, va( "%s %s %d",
        QQ( N_( "^3elo_dbg_printinventoryvalue: ^*$1$^*'s inventory is worth around $2$, according to our experts" ) ),
        Quote( ( target ) ? target->client->pers.netname : "a paranormal ghost" ),
        loadout_value
    ) );
}
