#pragma once

#include "GameTypes.h"


namespace StructsT8
{
	enum TKMovesetHeaderBlocks_
	{
		TKMovesetHeaderBlocks_MovesetInfo = 0,
		TKMovesetHeaderBlocks_Table       = 1,
		TKMovesetHeaderBlocks_Motalists   = 2,
		TKMovesetHeaderBlocks_Name        = 3,
		TKMovesetHeaderBlocks_Moveset     = 4,
		TKMovesetHeaderBlocks_Animation   = 5,
		TKMovesetHeaderBlocks_Mota        = 6,
		TKMovesetHeaderBlocks_Movelist    = 7,
	};

	// Custom-made offset list containing the list of blocks that compose our extracted moveset
	// These are arbitrary blocks and do not match the game's moveset 1:1
	struct TKMovesetHeaderBlocks
	{
		union {
			struct {
				// Store moveid aliases and some more i don't understand
				uint64_t movesetInfoBlock;
				// Store a list of offsets toward various structure lists paired with their size
				uint64_t tableBlock;
				// Store a list of offets pointing to mota files
				uint64_t motalistsBlock;
				// Stores an offset to the block containing move names & anim names (they are the same)
				uint64_t nameBlock;
				// Stores an offset to the block containing stucture lists
				uint64_t movesetBlock;
				// Store an offset to the block containing every regular animation used in the moveset. (customly-built)
				uint64_t animationBlock;
				// Store an offset to the block containing the multiple mota files for those that are extracted . (customly-built)
				uint64_t motaBlock;
				// Stores the .mvl file in order to show the custom movelist in training mode
				uint64_t movelistBlock;
			};
			uint64_t blocks[8];
		};

		Byte* GetBlock(TKMovesetHeaderBlocks_ block, Byte* moveset) const
		{
			return moveset + blocks[(unsigned int)block];
		}

		const Byte* GetBlock(TKMovesetHeaderBlocks_ block, const Byte* moveset) const
		{
			return moveset + blocks[(unsigned int)block];
		}

		uint64_t GetBlockSize(TKMovesetHeaderBlocks_ block, uint64_t s_moveset)
		{
			unsigned int b = (unsigned int)block;
			if ((b + 1) >= (unsigned int)_countof(blocks)) {
				return s_moveset - blocks[block];
			}
			else {
				return blocks[block + 1] - blocks[block];
			}
		}
	};

	union Param {
		uint32_t param_unsigned;
		int32_t param_signed;
		float param_float;
	};

	struct EncryptedValue
	{
		uint64_t value;
		uint64_t key;
	};

	struct MoveHitbox
	{
		uint32_t startup;
		uint32_t recovery;
		uint32_t location;
		float related_floats[9];
	};

	struct MoveHitboxUnknown // offset 0x2E4 of a move
	{
		int _0x0[3];    // offset 0x0
		float _0xC[3];  // offset 0xC
		uint32_t _0x14; // offset 0x14
		float _0x18[3]; // offset 0x18
		uint32_t _0x24; // offset 0x24
	};

	// -- Main data type -- //

	struct PushbackExtradata
	{
		int16_t displacement;
	};

	struct Pushback
	{
		uint16_t duration;
		uint16_t displacement;
		uint32_t offsets_count; // Number of Pushback Extradatas
		PushbackExtradata* extradata_addr;
	};

	struct Requirement
	{
		uint32_t condition;
		Param param[4];
	};

	struct CancelExtradata
	{
		uint32_t value;
	};

	struct Cancel
	{
		union
		{
			uint64_t command;
			struct
			{
				uint32_t direction;
				uint32_t button;
			};
		};
		Requirement* requirements_addr;
		CancelExtradata* extradata_addr;
		uint32_t detection_start;
		uint32_t detection_end;
		uint32_t starting_frame;
		uint16_t move_id;
		uint16_t cancel_option;
	};

	struct Reactions
	{
		union {
			struct {
				Pushback* front_pushback;
				Pushback* backturned_pushback;
				Pushback* left_side_pushback;
				Pushback* right_side_pushback;
				Pushback* front_counterhit_pushback; // If you ever wondered why your CH launcher didn't launch after a sidestep, that's why
				Pushback* downed_pushback;
				Pushback* block_pushback;
			};
			Pushback* pushbacks[7];
		};
		// Directions
		uint16_t front_direction;
		uint16_t back_direction;
		uint16_t left_side_direction;
		uint16_t right_side_direction;
		uint16_t front_counterhit_direction;
		uint16_t downed_direction;
		// Rotations
		uint16_t front_rotation;
		uint16_t back_rotation;
		uint16_t left_side_rotation;
		uint16_t right_side_rotation;
		uint16_t front_counterhit_rotation;
		uint16_t downed_rotation;

		union {
			struct {
				uint16_t default_moveid; // Standing move ID
				uint16_t crouch_moveid;
				uint16_t counterhit_moveid;
				uint16_t crouch_counterhit_moveid;
				uint16_t left_side_moveid;
				uint16_t crouch_left_side_moveid;
				uint16_t right_side_moveid;
				uint16_t crouch_right_side_moveid;
				uint16_t backturned_moveid;
				uint16_t crouch_backturned_moveid;
				uint16_t block_moveid;
				uint16_t crouch_block_moveid;
				uint16_t wallslump_moveid;
				uint16_t downed_moveid;
				uint16_t _unk1;
				uint16_t _unk2;
			};
			uint16_t moveids[16];
		};
	};

	struct HitCondition
	{
		Requirement* requirements_addr;
		uint32_t damage;
		uint32_t _0xC_int; // What the hell is this? since when has this been here?
		Reactions* reactions_addr;
	};

	struct Voiceclip
	{
		uint32_t folder; // folder of voice
  	uint32_t val2;
  	uint32_t clip; // ID of the clip
	};

	struct ExtraMoveProperty
	{
		uint32_t starting_frame;
		uint32_t _0x4_int;
		Requirement* requirements_addr;
		uint32_t id;
		Param param[5];
	};

	struct Input
	{
		union
		{
			uint64_t command;
			struct
			{
				uint32_t direction;
				uint32_t button;
			};
		};
	};

	struct InputSequence
	{
		uint16_t input_window_frames; // I assume the max amount of frames between the first &last input
		uint16_t input_amount; // The amount of input to read from input_addr
		int32_t _0x4_int; // Apparently unused
		Input* input_addr;
	};

	// TODO: Study and add proper fields later
	struct Projectile
	{
		uint32_t u1[35];             // Offset: 0x0
		HitCondition* hit_condition_addr; // Offset: 0x90
		Cancel* cancel_addr;        // Offset: 0x98
		uint32_t u2[16];
	};

	struct CameraData
	{
		uint32_t pick_probability;
		uint16_t camera_type;
		uint16_t left_side_camera_data;
		uint16_t right_side_camera_data;
		uint16_t additional_rotation;
	};

	struct ThrowCamera
	{
		uint64_t side; // Side at which throw recovers
		CameraData* cameradata_addr;
	};

	struct ParryAbleMove
	{
		uint32_t value;
	};

	// Struct for Extra Move Properties that play when a move starts or ends
	struct OtherMoveProperty
	{
		Requirement* requirements_addr;
		uint32_t extraprop; // 1100 list end value & extraprop values
		Param params[5];
	};

	struct DialogueManager
	{
		uint16_t type; // 0 = Intro, 1 = Outro, 2 = Fate
		uint16_t id;
		uint32_t _0x4;
		Requirement* requirements_addr;
		uint32_t voiceclip_key;
		uint32_t facial_anim_idx;
	};

	struct Move
	{
		EncryptedValue name_key;        // offset 0x0 // Done in patch 1.09
		uint32_t name_key_related[4];   // offset 0x10

		EncryptedValue anim_name_key;        // offset 0x20
		uint32_t anim_name_key_related[4];   // offset 0x30

		char *name_addr;                // offset 0x40 - no longer used
		char *anim_name_addr;           // offset 0x48 - no longer used
		uint32_t anim_key1;             // offset 0x50
		uint32_t anim_key2;             // offset 0x54 - Value always EF00XX00. XX is character ID

		EncryptedValue vuln;            // offset 0x58 // Hurtboxes
		uint32_t vuln_related[4];       // offset 0x68

		EncryptedValue hitlevel;        // offset 0x78
		uint32_t hitlevel_related[4];   // offset 0x88

		Cancel *cancel_addr;            // offset 0x98 // Seems to be the only one used, rest are unused.
		Cancel *cancel1_addr;           // offset 0xA0
		int32_t cancel1_related;        // offset 0xA8
		int32_t cancel1_related2;       // offset 0xAC
		Cancel *cancel2_addr;           // offset 0xB0
		int32_t cancel2_related;        // offset 0xB8
		int32_t cancel2_related2;       // offset 0xBC
		Cancel *cancel_addr3;           // offset 0xC0
		uint32_t cancel3_related;       // offset 0xC8
		uint16_t transition;            // offset 0xCC
		uint16_t _0xCE_short;           // offset 0xCE

		EncryptedValue ordinal_id1;     // offset 0xD0 // Used for all parries?
		uint32_t ordinal_id1_related[4]; // offset 0xE0

		EncryptedValue ordinal_id2;     // offset 0xF0
		uint32_t ordinal_id2_related[4]; // offset 0x100

		HitCondition* hit_condition_addr; // offset 0x110 // Requirement, Damage, Which Reaction List to perform.
		uint32_t _0x118;                // offset 0x118
		uint32_t _0x11C;                // offset 0x11C
		uint32_t anim_len;              // offset 0x120
		uint32_t airborne_start;        // offset 0x124
		uint32_t airborne_end;          // offset 0x128
		uint32_t ground_fall;           // offset 0x12C
		Voiceclip* voicelip_addr;       // offset 0x130 Can be NULL
		ExtraMoveProperty* extra_move_property_addr; // offset 0x138 Can be NULL
		OtherMoveProperty* move_start_extraprop_addr; // offset 0x140 Can be NULL
		OtherMoveProperty* move_end_extraprop_addr; // offset 0x148 Can be NULL
		uint32_t u15;                   // offset 0x150 extra body properties such as neck tracking/combo counter etc....
		uint32_t _0x154;                // offset 0x154
		uint32_t first_active_frame;    // offset 0x158
		uint32_t last_active_frame;     // offset 0x15C
		MoveHitbox hitboxes[8];         // offset 0x160 - 0x2E0
		uint32_t _0x2E0;                // offset 0x2E0
		MoveHitboxUnknown _0x2E4[8];    // offset 0x2E4 - 0x440
		uint32_t _0x444;                // offset 0x444
	};

	// -- Other -- //

	struct MovesetTable
	{
		Reactions* reactions;
		uint64_t _0x8; // what if this was wrongly placed?
		uint64_t reactionsCount;

		union {
			struct {
				Requirement* requirement;
				uint64_t requirementCount;

				HitCondition* hitCondition;
				uint64_t hitConditionCount;

				Projectile* projectile;
				uint64_t projectileCount;

				Pushback* pushback;
				uint64_t pushbackCount;

				PushbackExtradata* pushbackExtradata;
				uint64_t pushbackExtradataCount;

				Cancel* cancel;
				uint64_t cancelCount;

				Cancel* groupCancel;
				uint64_t groupCancelCount;

				CancelExtradata* cancelExtradata;
				uint64_t cancelExtradataCount;

				ExtraMoveProperty* extraMoveProperty;
				uint64_t extraMovePropertyCount;

				OtherMoveProperty* moveBeginningProp; // Extraprops that play when a move starts
				uint64_t moveBeginningPropCount;

				OtherMoveProperty* moveEndingProp; // Extraprops that play when a move ends
				uint64_t moveEndingPropCount;

				Move* move;
				uint64_t moveCount;

				Voiceclip* voiceclip;
				uint64_t voiceclipCount;

				InputSequence* inputSequence;
				uint64_t inputSequenceCount;

				Input* input;
				uint64_t inputCount;

				ParryAbleMove* parryableMoves;
				uint64_t parryableMovesCount;

				CameraData* cameraData;
				uint64_t cameraDataCount;

				ThrowCamera* throwCameras;
				uint64_t throwCamerasCount;

				DialogueManager* dialoguesData;
				uint64_t dialoguesDataCount;
			};
		};
		struct {
			void* listAddr;
			uint64_t listCount;
		} entries[19];
	};

	struct MotaHeader
	{
		char mota_string[4];
		bool _is_little_endian; // The first 2 bytes after the string are always 0x0001 or 0x0100
		bool _is_big_endian; // So they can be used to determine endian but they shouldn't, they might be used for different purposes somewhere
		uint16_t unk0x6;
		uint32_t unk0x8;
		uint32_t anim_count;
		char unk0x10;
		bool is_little_endian; // When the game checks for Mota endian, it checks this byte specifically and not the one above
		char unk0x13;
		char unk0x14;
		unsigned int anim_offset_list[1];


		bool IsValid() const
		{
			return strncmp(mota_string, "MOTA", 4) == 0;
		}

		bool IsValid(uint64_t size) const
		{
			return size >= (sizeof(MotaHeader) - 4) && IsValid();
		}

		bool IsBigEndian() const
		{
			return !is_little_endian;
		}
	};

	// MOTAs appear to be gone in Tk8. For now, this struct is useless :(
	struct MotaList
	{
		union {
			struct {
					MotaHeader* mota_0; // Anims
					MotaHeader* mota_1; // Anims
					MotaHeader* hand_1; // Hand
					MotaHeader* hand_2; // Hand
					MotaHeader* face_1; // Face
					MotaHeader* face_2; // Face
					MotaHeader* wings_1; // Wings (probably more to it)
					MotaHeader* wings_2; // Wings (probably more to it)
					MotaHeader* camera_1; // Camera
					MotaHeader* camera_2; // Camera
					MotaHeader* mota_10; // Unknown
					MotaHeader* mota_11; // Unknown
					MotaHeader* _unknown_; // Points to start of the file that contains all odd-numbered MOTAs
			};
			MotaHeader* motas[13];
		};
	};

	struct MovesetInfo
	{
		char _0x0[2];
		bool isInitialized;
		bool _0x3;
		char _0x4[4];
		char _0x8[4]; // string "TEK"
		uint32_t _0xC;
		char* character_name_addr;
		char* character_creator_addr;
		char* date_addr;
		char* fulldate_addr;
		uint16_t orig_aliases[60];
		uint16_t current_aliases[60];
		uint16_t unknown_values[38];
		MovesetTable table;
		MotaList motas;
	};

	// Displayable movelist structures

	// Inputs played by bot in movelist display
	struct MvlInput //todo
	{
		union {
			struct {
				Byte directions;
				Byte buttons;
			};
			uint16_t command;
		};
		uint8_t frame_duration;
		uint8_t trigger_highlight;
	};

	// .mvl file
	struct MvlHead //todo
	{
		char mvlString[4];
		int32_t _unk0x4;
		int32_t displayables_count;
		int32_t playables_count;
		int32_t _unk0x10;
		uint32_t displayables_offset;
		uint32_t playables_offset;
		uint32_t inputs_offset;
		char translation_strings[1];
	};

	typedef uint32_t MvlDisplayableType;
	enum MvlDisplayableType_
	{
		MvlDisplayableType_Disabled       = 0x2,
		MvlDisplayableType_Category       = 0x8,
		MvlDisplayableType_Regular        = 0x10000,
		MvlDisplayableType_ComboListStart = 0xA,
		MvlDisplayableType_Unknown        = 0xD,
	};

	struct MvlDisplayable //todo
	{
		union {
			// Normally, these are uints in files and negative ints in memory, both in our software and in the actual game
			// They are never really high though so we can just store them as ints all the times with no worries
			struct {
				int32_t title_translation_japanese;
				int32_t title_translation_english;
				int32_t title_translation_japanese_2;
				int32_t title_translation_other;
				int32_t translation_offsets_japanese;
				int32_t translation_offsets_english;
				int32_t translation_offsets_portuguese;
				int32_t translation_offsets_spanish;
				int32_t translation_offsets_french;
				int32_t translation_offsets_italian;
				int32_t translation_offsets_german;
				int32_t translation_offsets_mexican;
				int32_t translation_offsets_cyrillic;
				int32_t translation_offsets_arabic;
				int32_t translation_offsets_chinese;
				int32_t translation_offsets_korean;
			};
			struct {
				int32_t title_translation_offsets[4];
				int32_t translation_offsets[12];
			};
			int32_t all_translation_offsets[16];
		};
		uint32_t _unk0x40;
		int16_t playable_id;
		uint16_t _unk0x46;
		MvlDisplayableType type;
		uint32_t _unk0x4c;
		uint32_t _unk0x50;
		uint32_t _unk0x54;
		uint32_t _unk0x58;
		uint32_t _unk0x5c;
		uint32_t _unk0x60;
		uint32_t _unk0x64;
		uint32_t _unk0x68;
		uint32_t _unk0x6c;
		uint32_t _unk0x70;
		uint32_t _unk0x74;
		uint32_t _unk0x78;
		uint32_t _unk0x7c;
		uint32_t _unk0x80;
		uint32_t _unk0x84;
		uint32_t _unk0x88;
		uint32_t _unk0x8c;
		uint32_t _unk0x90;
		uint32_t _unk0x94;
		uint32_t _unk0x98;
		uint32_t _unk0x9c;
		uint32_t _unk0xa0;
		uint32_t _unk0xa4;
		uint32_t _unk0xa8;
		uint32_t _unk0xac;
		uint32_t _unk0xb0;
		uint32_t _unk0xb4;
		uint32_t _unk0xb8;
		uint32_t _unk0xbc;
		uint32_t _unk0xc0;
		uint32_t _unk0xc4;
		uint32_t _unk0xc8;
		uint32_t _unk0xcc;
		uint32_t _unk0xd0;
		uint32_t _unk0xd4;
		uint32_t _unk0xd8;
		uint32_t _unk0xdc;
		uint32_t _unk0xe0;
		uint32_t _unk0xe4;
		uint32_t _unk0xe8;
		uint32_t _unk0xec;
		uint32_t _unk0xf0;
		uint32_t _unk0xf4;
		uint32_t _unk0xf8;
		uint32_t _unk0xfc;
		uint32_t _unk0x100;
		uint32_t _unk0x104;
		uint32_t _unk0x108;
		uint32_t _unk0x10c;
		uint32_t _unk0x110;
		uint32_t _unk0x114;
		uint32_t _unk0x118;
		uint32_t _unk0x11c;
		uint32_t _unk0x120;
		uint32_t _unk0x124;
		uint32_t _unk0x128;
		uint32_t _unk0x12c;
		uint32_t _unk0x130;
		uint32_t _unk0x134;
		uint32_t _unk0x138;
		uint32_t _unk0x13c;
		uint32_t _unk0x140;
		uint32_t _unk0x144;
		uint32_t _unk0x148;
		uint32_t icons;
		uint8_t icons_2;
		uint8_t combo_damage;
		uint8_t combo_difficulty;
		uint8_t _unk0x153;
		uint32_t _unk0x154;
		uint32_t _unk0x158;
		uint32_t _unk0x15c;
		uint32_t _unk0x160;
		uint32_t _unk0x164;
		uint32_t _unk0x168;
		uint32_t _unk0x16c;
		uint32_t _unk0x170;
	};


	struct MvlPlayable //todo
	{
		uint16_t p2_action;
		uint16_t distance;
		uint16_t p2_rotation;
		uint16_t _unk0x6;
		uint16_t _unk0x8;
		uint16_t p1_facing_related;
		uint16_t _unk0xc;
		uint16_t input_count;
		uint32_t input_sequence_offset;
		uint16_t has_rage;
		uint16_t _unk0x16;
	};

	// Struct that manages the training mode movelist in memory
	struct MvlManager //todo
	{
		uint32_t playingType;
		uint32_t _unk0x4;
		void* player;
		uint32_t displayableEntriesCount;
		uint32_t _unk0x14;
		void* mvlDisplayableBlock;
		MvlHead* mvlHead;
		uint32_t _unk0x28;
		uint32_t _unk0x2c;
		uint32_t selected_entry;
		uint32_t _unk0x34;
		uint32_t _unk0x38;
		uint32_t _unk0x3c;
		uint32_t _unk0x40;
		uint32_t _unk0x44;
		uint32_t _unk0x48;
		uint32_t _unk0x4c;
		uint32_t _unk0x50;
		uint32_t _unk0x54;
		uint32_t _unk0x58;
		uint32_t _unk0x5c;
		MvlInput* sequenceEnd;
		MvlInput* sequenceStart;
		int32_t frame;
		int32_t currentInputFrame;
		uint32_t _unk0x78;
		uint32_t _unk0x7c;
	};
}


// Contains most of the same structs as above, but with no ptr (gameAddr instead)
// Helps reduce the need of casting throughout the code and that is important for code clarity
namespace StructsT8_gameAddr //todo
{
	union Param {
		uint32_t param_unsigned;
		int32_t param_signed;
		float param_float;
	};

	struct Pushback
	{
		uint16_t duration;
		uint16_t displacement;
		uint32_t offsets_count; // Number of Pushback Extradatas
		gameAddr extradata_addr;
	};

	struct Cancel
	{
		union
		{
			uint64_t command;
			struct
			{
				uint32_t direction;
				uint32_t button;
			};
		};
		gameAddr requirements_addr;
		gameAddr extradata_addr;
		uint32_t detection_start;
		uint32_t detection_end;
		uint32_t starting_frame;
		uint16_t move_id;
		uint16_t cancel_option;
	};

	struct Reactions
	{
		union {
			struct {
				gameAddr front_pushback;
				gameAddr backturned_pushback;
				gameAddr left_side_pushback;
				gameAddr right_side_pushback;
				gameAddr front_counterhit_pushback; // If you ever wondered why your CH launcher didn't launch after a sidestep, that's why
				gameAddr downed_pushback;
				gameAddr block_pushback;
			};
			gameAddr pushbacks[7];
		};

		uint16_t front_direction;
		uint16_t back_direction;
		uint16_t left_side_direction;
		uint16_t right_side_direction;
		uint16_t front_counterhit_direction;
		uint16_t downed_direction;

		uint16_t front_rotation;
		uint16_t back_rotation;
		uint16_t left_side_rotation;
		uint16_t right_side_rotation;
		uint16_t front_counterhit_rotation;
		uint16_t downed_rotation;

		union {
			struct {
				uint16_t default_moveid; // Standing move ID
				uint16_t crouch_moveid;
				uint16_t counterhit_moveid;
				uint16_t crouch_counterhit_moveid;
				uint16_t left_side_moveid;
				uint16_t crouch_left_side_moveid;
				uint16_t right_side_moveid;
				uint16_t crouch_right_side_moveid;
				uint16_t backturned_moveid;
				uint16_t crouch_backturned_moveid;
				uint16_t block_moveid;
				uint16_t crouch_block_moveid;
				uint16_t wallslump_moveid;
				uint16_t downed_moveid;
				uint16_t _unk1;
				uint16_t _unk2;
			};
			uint16_t moveids[16];
		};

		// Some unused and unaccessed 4 bytes
		// Probably there because of C structure padding
		// Constantly at 0
		unsigned int _0x6c;
	};

	struct HitCondition
	{
		gameAddr requirements_addr;
		uint32_t damage;
		uint32_t _0xC_int; // What the hell is this? since when has this been here?
		gameAddr reactions_addr;
	};

	struct InputSequence
	{
		uint16_t input_window_frames; // I assume the max amount of frames between the first &last input
		uint16_t input_amount; // The amount of inputs to read from inputAddr
		int32_t _0x4_int; // Apparently unused
		gameAddr input_addr;
	};

	// TODO: Fix it later
	struct Projectile
	{
		uint32_t u1[35];             // Offset: 0x0
		gameAddr hit_condition_addr; // Offset: 0x90
		gameAddr cancel_addr;        // Offset: 0x98
		uint32_t u2[16];
	};


	struct ThrowCamera
	{
		uint64_t side;
		gameAddr cameradata_addr;
	};

	struct OtherMoveProperty
	{
		gameAddr requirements_addr;
		uint32_t extraprop; // 1100 list end value & extraprop values
		Param param[5];
	};

	struct ExtraMoveProperty
	{
		uint32_t starting_frame;
		uint32_t _0x4_int;
		gameAddr requirements_addr;
		uint32_t id;
		Param param[5];
	};


	struct DialogueManager
	{
		uint16_t type; // 0 = Intro, 1 = Outro, 2 = Fate
		uint16_t id;
		uint32_t _0x4;
		gameAddr requirements_addr;
		uint32_t voiceclip_key;
		uint32_t facial_anim_idx;
	};

	struct Move //todo
	{
		EncryptedValue name_key;        // offset 0x0
		uint32_t name_key_related[4];   // offset 0x10

		EncryptedValue anim_name_key;        // offset 0x20
		uint32_t anim_name_key_related[4];   // offset 0x30

		char *name_addr;                // offset 0x40 - no longer used
		char *anim_name_addr;           // offset 0x48 - no longer used
		uint32_t anim_key1;             // offset 0x50
		uint32_t anim_key2;             // offset 0x54 - Value always EF00XX00. XX is character ID

		EncryptedValue vuln;            // offset 0x58
		uint32_t vuln_related[4];       // offset 0x68

		EncryptedValue hitlevel;        // offset 0x78
		uint32_t hitlevel_related[4];   // offset 0x88

		gameAddr cancel_addr;           // offset 0x98
		gameAddr cancel1_addr;          // offset 0xA0
		int32_t cancel1_related;        // offset 0xA8
		int32_t cancel1_related2;       // offset 0xAC
		gameAddr cancel2_addr;          // offset 0xB0
		int32_t cancel2_related;        // offset 0xB8
		int32_t cancel2_related2;       // offset 0xBC
		gameAddr cancel_addr3;          // offset 0xC0
		uint32_t cancel3_related;       // offset 0xC8
		uint16_t transition;            // offset 0xCC
		uint16_t _0xCE_short;           // offset 0xCE

		EncryptedValue ordinal_id1;     // offset 0xD0
		uint32_t ordinal_id1_related[4]; // offset 0xE0

		EncryptedValue ordinal_id2;     // offset 0xF0
		uint32_t ordinal_id2_related[4]; // offset 0x100

		gameAddr hit_condition_addr;    // offset 0x110
		uint32_t _0x118;                // offset 0x118
		uint32_t _0x11C;                // offset 0x11C
		uint32_t anim_len;              // offset 0x120
		uint32_t airborne_start;        // offset 0x124
		uint32_t airborne_end;          // offset 0x128
		uint32_t ground_fall;           // offset 0x12C
		gameAddr voicelip_addr;        // offset 0x130 Can be NULL
		gameAddr extra_move_property_addr; // offset 0x138 Can be NULL
		gameAddr move_start_extraprop_addr; // offset 0x140 Can be NULL
		gameAddr move_end_extraprop_addr; // offset 0x148 Can be NULL
		uint32_t u15;                   // offset 0x150 extra body properties such as neck tracking/combo counter etc....
		uint32_t _0x154;               // offset 0x154
		uint32_t first_active_frame;    // offset 0x158
		uint32_t last_active_frame;     // offset 0x15C
		MoveHitbox hitboxes[8];         // offset 0x160 - 0x2E0
		uint32_t _0x2E0;               // offset 0x2E0
		MoveHitboxUnknown _0x2E4[8];    // offset 0x2E4 - 0x440
		uint32_t _0x444;               // offset 0x444
	};

	// -- Other -- //

	struct MovesetTable
	{
		gameAddr reactions;
		uint64_t _0x8; // always 0 so far, goes unused it seems
		uint64_t reactionsCount;

		union {
			struct {
				gameAddr requirement;
				uint64_t requirementCount;

				gameAddr hitCondition;
				uint64_t hitConditionCount;

				gameAddr projectile;
				uint64_t projectileCount;

				gameAddr pushback;
				uint64_t pushbackCount;

				gameAddr pushbackExtradata;
				uint64_t pushbackExtradataCount;

				gameAddr cancel;
				uint64_t cancelCount;

				gameAddr groupCancel;
				uint64_t groupCancelCount;

				gameAddr cancelExtradata;
				uint64_t cancelExtradataCount;

				gameAddr extraMoveProperty;
				uint64_t extraMovePropertyCount;

				gameAddr moveBeginningProp; // Extraprops that play when a move starts
				uint64_t moveBeginningPropCount;

				gameAddr moveEndingProp; // Extraprops that play when a move ends
				uint64_t moveEndingPropCount;

				gameAddr move;
				uint64_t moveCount;

				gameAddr voiceclip;
				uint64_t voiceclipCount;

				gameAddr inputSequence;
				uint64_t inputSequenceCount;

				gameAddr input;
				uint64_t inputCount;

				gameAddr parryableMoves;
				uint64_t parryableMovesCount;

				gameAddr cameraData;
				uint64_t cameraDataCount;

				gameAddr throwCameras;
				uint64_t throwCamerasCount;

				gameAddr dialoguesData;
				uint64_t dialoguesDataCount;
			};
			struct {
				gameAddr listAddr;
				uint64_t listCount;
			} entries[19];
		};
	};

	// MOTAs appear to be gone in Tk8. For now, this struct is useless :(
	struct MotaList
	{
		union {
			struct {
				gameAddr mota_0; // Anims
				gameAddr mota_1; // Anims
				gameAddr mota_2; // Hand
				gameAddr mota_3; // Hand
				gameAddr mota_4; // Face
				gameAddr mota_5; // Face
				gameAddr mota_6; // Wings (probably more to it)
				gameAddr mota_7; // Wings (probablty more to it)
				gameAddr mota_8; // Camera
				gameAddr mota_9; // Camera
				gameAddr mota_10; // Unknown
				gameAddr mota_11; // Unknown
				gameAddr _unknown_; // Points to start of the file that contains all odd-numbered MOTAs
			};
			gameAddr motas[13];
		};
	};

	struct MovesetInfo
	{
		char _0x0[2];
		bool isInitialized;
		bool _0x3;
		char _0x4[4];
		char _0x8[4]; // string "TEK"
		uint32_t _0xC;
		char* character_name_addr;
		char* character_creator_addr;
		char* date_addr;
		char* fulldate_addr;
		uint16_t orig_aliases[60];
		uint16_t current_aliases[60];
		uint16_t unknown_aliases[38];
		MovesetTable table;
		MotaList motas;
	};
};
