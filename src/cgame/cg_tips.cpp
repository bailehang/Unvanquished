#include "cg_local.h"
#include "cg_tips.h"
#include <common/FileSystem.h>

std::string chosen_tip;
bool tips_initialized = false;

std::vector<std::string> tips;
int tip_choice;

void CG_Init_Tips()
{
	if ( tips_initialized ) { return; } // do only once per vm initialization

	// these are used for both default tipfiles and map tipfiles
	std::string infile;
	std::string newtip;
	size_t curpos;

	char language[ 32 ];
	trap_Cvar_VariableStringBuffer( "language", language, sizeof( language ) );
	Log::Debug( "CG_Init_Tips: language = %s", language );

	std::string tipfile_default; // this will change if successful
	std::string tipfile_default_l10n;

	// this would set tipfile_default_l10n to "tips/default_en.txt", assuming language is "en"
	char tipfile_default_l10n_cstr[ 32 ];
	sprintf( tipfile_default_l10n_cstr, "tips/default_%s.txt", language );
	tipfile_default_l10n = std::string( tipfile_default_l10n_cstr );

	if ( FS::PakPath::FileExists( tipfile_default_l10n ) )
	{
		tipfile_default = tipfile_default_l10n;
	}
	else if ( FS::PakPath::FileExists( "tips/default.txt" ) )
	{
		// fallback to just "tips/default.txt" in case of older LTWS packs
		tipfile_default = "tips/default.txt";
		// ...but complain about it because come on bro
		Log::Warn( "Default tips file contains no localization tag" );
	}

	if ( !tipfile_default.empty() ) {
		Log::Debug( "CG_Init_Tips: default tip file found: %s", tipfile_default );
		infile = FS::PakPath::ReadFile( tipfile_default );
		newtip = "";
		curpos = 0;
		while ( ( curpos = infile.find('\n') ) != std::string::npos )
		{
			// split by \n
			newtip = infile.substr(0, curpos);
			tips.push_back(newtip);

			// erase from buffer
			infile.erase(0, curpos + 1);
		}
	}
	
	// per-map tips
	// tip file search order (assuming yocto and french):
	//	 - tips/yocto_fr.txt
	//	 - tips/yocto_en.txt (fallback)
	//	 - tips/yocto.txt    (no l10n, fallback)
	std::string tipfile_map;
	std::string tipfile_map_l10n;
	std::string tipfile_map_eng;
	std::string tipfile_map_solo;

	char tipfile_map_l10n_cstr[ 32 ];
	sprintf( tipfile_map_l10n_cstr, "tips/%s_%s.txt", cgs.mapname, language );
	tipfile_map_l10n = std::string( tipfile_map_l10n_cstr );

	char tipfile_map_eng_cstr[ 32 ];
	sprintf( tipfile_map_eng_cstr, "tips/%s_en.txt", cgs.mapname );
	tipfile_map_eng = std::string( tipfile_map_eng_cstr );

	char tipfile_map_solo_cstr[ 32 ];
	sprintf( tipfile_map_solo_cstr, "tips/%s.txt", cgs.mapname );
	tipfile_map_solo = std::string( tipfile_map_solo_cstr );

	if ( FS::PakPath::FileExists( tipfile_map_l10n ) )
	{
		tipfile_map = tipfile_map_l10n;
	}
	else if ( !strcmp( language, "en" ) && FS::PakPath::FileExists( tipfile_map_eng ) )
	{
		tipfile_map = tipfile_map_eng;
	}
	else if ( FS::PakPath::FileExists( tipfile_map_solo ) )
	{
		tipfile_map = tipfile_map_solo;
	}

	// finally, if we're successful, include tips in the potential list
	if ( !tipfile_map.empty() )
	{
		Log::Debug( "CG_Init_tips: map tip file found: %s", tipfile_map );
		infile = FS::PakPath::ReadFile( tipfile_map );
		newtip = "";
		curpos = 0;
		while ( ( curpos = infile.find('\n') ) != std::string::npos )
		{
			// split by \n
			newtip = infile.substr(0, curpos);
			tips.push_back(newtip);

			// erase from buffer
			infile.erase(0, curpos + 1);
		}
	}


	tip_choice = rand() / (RAND_MAX / tips.size() + 1);
	tips_initialized = true;
}
	
