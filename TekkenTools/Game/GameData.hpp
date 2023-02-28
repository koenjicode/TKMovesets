#pragma once

#include <map>
#include <string>

#include "GameProcess.hpp"

// Class that allows you to read from the game using only the address identifier from game_address.txt
// Also provides caching of pointer paths. But overall this is still a high-level class aimed at cleaning up code elsewhere and keeping things segmented.
// Todo: store current player datas here for fancy displaying later on
class GameData
{
private:
	GameData() = default;
	~GameData() = default;
	GameData& operator = (const GameData&) = delete;
	GameData(const GameData&) = delete;

	// Cached module address to avoid having to fetch it all the time
	int64_t moduleAddress{};
	// Cached addresses and pointer path so that we don't have to re-compute everything mid-extraction/importation
	std::map<std::string, void*> m_cachedAddresses;
	// The singleton that allows us to read/write on process memory
	GameProcess* process;
	
	// Reads a ptr path from an address identifier and return its last pointer
	void* ReadPtrPath(const char* c_addressId);
public:
	static GameData& getInstance() {
		// Todo: mutex here or something?
		static GameData s_instance;
		return s_instance;
	}

	// Reads the addresses file and compute every address from their pointer path (when possible) to avoid having to do it later
	void CacheAddresses();

	// Reads a char (1b) from the game in little endian
	int8_t ReadInt8(const char* c_addressId);
	// Reads a short (2b) from the game in little endian
	int16_t ReadInt16(const char* c_addressId);
	// Reads an int (4b) from the game in little endian
	int32_t ReadInt32(const char* c_addressId);
	// Reads an int (4b) from the game in little endian
	int64_t ReadInt64(const char* c_addressId);
	// Reads a floating point number (4b) from the game in little endian
	float ReadFloat(const char* c_addressId);
	// Reads [readSize] amounts of bytes from the game and write them to the provided buffer
	void ReadBytes(const char* c_addressId, void* buf, size_t readSize);

	// Todo: writing functions
};