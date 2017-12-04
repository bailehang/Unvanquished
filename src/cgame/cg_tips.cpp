#include "cg_local.h"
#include "cg_tips.h"
#include <common/FileSystem.h>

std::string chosen_tip;
bool tips_initialized = false;

std::vector<std::string> tips;
int tip_choice;
char language[ 32 ];

std::vector<std::string> CG_Get_Tips( const char* target )
{
    std::vector<std::string> result;
	// tip file search order (assuming yocto and french):
	//	 - tips/yocto_fr.txt
	//	 - tips/yocto_en.txt (fallback)
	//	 - tips/yocto.txt    (no l10n, fallback)
	std::string tipfile_target;
	std::string tipfile_target_l10n;
	std::string tipfile_target_eng;
	std::string tipfile_target_solo;

	char tipfile_target_l10n_cstr[ 32 ];
	sprintf( tipfile_target_l10n_cstr, "tips/%s_%s.txt", target, language );
	tipfile_target_l10n = std::string( tipfile_target_l10n_cstr );

	char tipfile_target_eng_cstr[ 32 ];
	sprintf( tipfile_target_eng_cstr, "tips/%s_en.txt", target );
	tipfile_target_eng = std::string( tipfile_target_eng_cstr );

	char tipfile_target_solo_cstr[ 32 ];
	sprintf( tipfile_target_solo_cstr, "tips/%s.txt", target );
	tipfile_target_solo = std::string( tipfile_target_solo_cstr );

	if ( FS::PakPath::FileExists( tipfile_target_l10n ) )
	{
		tipfile_target = tipfile_target_l10n;
	}
	else if ( !strcmp( language, "en" ) && FS::PakPath::FileExists( tipfile_target_eng ) )
	{
		tipfile_target = tipfile_target_eng;
	}
	else if ( FS::PakPath::FileExists( tipfile_target_solo ) )
	{
		tipfile_target = tipfile_target_solo;
	}

	// finally, if we're successful, include tips in the potential list
	if ( !tipfile_target.empty() )
	{
		Log::Debug( "CG_Init_tips: map tip file found: %s", tipfile_target );
		std::string infile = FS::PakPath::ReadFile( tipfile_target );
		std::string newtip = "";
		size_t curpos = 0;
		while ( ( curpos = infile.find('\n') ) != std::string::npos )
		{
			// split by \n
			newtip = infile.substr(0, curpos);
			result.push_back(newtip);

			// erase from buffer
			infile.erase(0, curpos + 1);
		}
	}

    return result;
}

void CG_Init_Tips()
{
	if ( tips_initialized ) { return; } // do only once per vm initialization

	trap_Cvar_VariableStringBuffer( "language", language, sizeof( language ) );
	Log::Debug( "CG_Init_Tips: language = %s", language );

    std::vector<std::string> default_tips = CG_Get_Tips( "default" );
    tips.insert( tips.end(), default_tips.begin(), default_tips.end() );    // for all maps
    std::vector<std::string> map_tips = CG_Get_Tips( cgs.mapname );
    tips.insert( tips.end(), map_tips.begin(), map_tips.end() );            // for current map

	tip_choice = rand() / (RAND_MAX / tips.size() + 1);
	tips_initialized = true;
}
	
