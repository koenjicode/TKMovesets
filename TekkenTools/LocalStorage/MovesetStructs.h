#pragma once

#include <stdint.h>

struct MovesetHeader_infos
{
	// ID of the game that the moveset was extracted from
	int32_t gameId;
	// Version string of the extracted moveset
	char version_string[28];
	// ID of the extracted character, used internally to make some moves that require it work
	int32_t characterId;
	// Flags used for storing useful data. Currently unused. Todo : see what we can do with this?
	int64_t flags;
	// Origin (Game name + author if you want)
	char origin[32];
	// Target character to play on
	char target_character[32];
	// Date of last modification
	char date[24];
};

struct MovesetHeader_offsets
{
	// Offsets of the various blocks contained within the moveset, in the same order they will appear in
	int64_t movesetInfoBlockOffset;
	int64_t listsBlockOffset;
	int64_t nameBlockOffset;
	int64_t movesetBlockOffset;
	int64_t animationBlockOffset; // Custom block
	int64_t motaBlockOffset; // Custom block
};


// Data contained at the head of every extracted movesets
struct MovesetHeader
{
	// Useful infos regarding the moveset : extracting date, game origin, game id, extractor version
	MovesetHeader_infos infos;
	// Absolute offsets of the moveset-related data blocks within the file
	MovesetHeader_offsets offsets;
};
