#pragma once

#include <string>

#include "Extractor.hpp"

#include "constants.h"
#include "Structs_t8.h"
#include "MovesetStructs.h"

// This class has no storage and i would like to keep it that way.
// You should be passing values through method calls and no other way.
// It should be able to call Export() while another Export() is going on in another thread and this without breaking anything

class ExtractorT8 : public Extractor
{
private:
	/// Main functions

	// Fill the moveset table & its corresponding offset table brother.
	void FillMovesetTables(gameAddr movesetAddr, StructsT8_gameAddr::MovesetTable* table, StructsT8_gameAddr::MovesetTable* offsets);
	// Fill moveset header with our own various useful informations
	void FillHeaderInfos(TKMovesetHeader& infos, gameAddr playerAddress, uint64_t propertyCount);
	// Allocate and copy the contents of the moveset block
	Byte* CopyMovesetBlock(gameAddr movesetAddr, uint64_t& size_out, const StructsT8_gameAddr::MovesetTable& table);
	// Allocate and copy the contents of the name block
	char* CopyNameBlock(gameAddr movesetAddr, uint64_t& size_out, const StructsT8_gameAddr::MovesetInfo& movesetHeader);
	// Fill motalist struct, build list of anims within the mota file, allocate and copy contents of the mota block
	Byte* CopyMotaBlocks(gameAddr movesetAddr, uint64_t& size_out, StructsT8::MotaList* motasList, ExtractSettings settings);
	// Returns an allocated block containing animations that weren't in the main animation block. Also builds a map to convert anim addresses to offsets.
	Byte* CopyAnimations(const StructsT8_gameAddr::Move* movelist, size_t moveCount, uint64_t& size_out, std::map<gameAddr, uint64_t>& offsets);
	// Copies the moveset info block (aliases, couple strings)
	void CopyMovesetInfoBlock(gameAddr movesetAddr, StructsT8_gameAddr::MovesetInfo* movesetHeader);
	// Copies the displayable movelist, visible in training mode and maybe elsewhere
	Byte* CopyDisplayableMovelist(gameAddr movesetAddr, gameAddr playerAddress, uint64_t& size_out, ExtractSettings settings);

	/// Helpers

	// Writes bounds of the block containing anim and move names
	void GetNamesBlockBounds(const StructsT8_gameAddr::Move* move, uint64_t moveCount, gameAddr& start, gameAddr& end);
	// Returns an allocated block containing mota block offsets followed by mota blocks
	Byte* AllocateMotaCustomBlock(StructsT8::MotaList* motas, uint64_t& size_out, ExtractSettings settings);
	// Calculate the size of the mota custom block we will build, and also fill boundaries with every animation address we can find
	uint64_t CalculateMotaCustomBlockSize(const StructsT8::MotaList* motas, std::map<gameAddr, std::pair<gameAddr, uint64_t>>& offsetMap, ExtractSettings settings);

	std::map<int, std::string> m_polarisCharacterNamesMap{ {0, "PAUL"}, {1, "LAW"}, {2, "KING"}, {3, "YOSHIMITSU"}, {4, "HWOARANG"},
		{5, "XIAOYU"}, {6, "JIN"}, {7, "BRYAN"}, {8, "KAZUYA"}, {9, "STEVE"},
		{10, "JACK-8"}, {11, "ASUKA"}, {12, "DEVIL_JIN"}, {13, "FENG"}, {14, "LILI"},
		{15, "DRAGUNOV"}, {16, "LEO"}, {17, "LARS"}, {18, "ALISA"}, {19, "CLAUDIO"},
		{20, "SHAHEEN"}, {21, "NINA"}, {22, "LEE"}, {23, "KUMA"}, {24, "PANDA"},
		{25, "ZAFINA"}, {26, "LEROY"}, {27, "JUN"}, {28, "REINA"}, {29, "AZUCENA"},
		{30, "VICTOR"}, {31, "RAVEN"}, {32, "AZAZEL"}, {33, "EDDY"}, {34, "LIDIA"},
		{35, "HEIHACHI"}, {36, "CLIVE"}, {37, "ANNA"}, {116, "DUMMY"}, {117, "ANGEL_JIN"},
		{118, "TRUE_DEVIL_KAZUYA"}, {119, "JACK-7"}, {120, "SOLDIER"},
		{121, "DEVIL_JIN_2"}, {122, "TEKKEN_MONK"}, {123, "SEIRYU"} };

public:
	using Extractor::Extractor; // Inherit constructor too
	ExtractionErrcode_ Extract(gameAddr playerAddress, ExtractSettings settings, uint8_t& progress) override;
	bool CanExtract() override;

	std::string GetPlayerCharacterName(gameAddr playerAddress) override;
	uint32_t GetCharacterID(gameAddr playerAddress) override;
	gameAddr GetCharacterAddress(uint8_t playerId) override;
	std::vector<gameAddr> GetCharacterAddresses() override;
};
