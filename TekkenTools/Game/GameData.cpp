#include <map>
#include <vector>

#include "GameData.hpp"
#include "GameProcess.hpp"
#include "GameAddresses.h"
#include "GameAddressesFile.hpp"

void GameData::CacheAddresses()
{
	std::map<std::string, gameAddr> cachedAddresses;

	for (std::string addressId : GameAddressesFile::GetAllEntries())
	{
		if (addressId.rfind("val_", 0) == 0) {
			// Ignore if not address / ptr path
			continue;
		}

		cachedAddresses[addressId] = ReadPtrPath(addressId.c_str());
	}
	m_cachedAddresses = cachedAddresses;
}

gameAddr GameData::ReadPtrPath(const char* c_addressId)
{
	bool isRelative;
	std::vector<gameAddr> ptrPath = GameAddressesFile::GetAddress(c_addressId, isRelative);

	if (ptrPath.size() == 0) {
		return GAME_ADDR_NULL;
	}

	gameAddr addr = ptrPath[0];
	if (isRelative) {
		addr += (int64_t)m_process->modBaseAddr;
	}

	{
		const size_t pathLen = ptrPath.size();
		if (pathLen > 1)
		{
			for (size_t i = 1; i < pathLen; ++i)
			{
				if (addr == (gameAddr)0) {
					return GAME_ADDR_NULL;
				}
				addr = (gameAddr)m_process->readInt64(addr) + (int64_t)ptrPath[i];
			}
			// double, triplecheck. Thre's no way this works right now
		}
	}

	return addr;
}

int8_t GameData::ReadInt8(const char* c_addressId)
{
	gameAddr addr = (m_cachedAddresses.find(c_addressId) == m_cachedAddresses.end())
		? ReadPtrPath(c_addressId) : m_cachedAddresses[c_addressId];
	return m_process->readInt8(addr);
}

int16_t GameData::ReadInt16(const char* c_addressId)
{
	gameAddr addr = (m_cachedAddresses.find(c_addressId) == m_cachedAddresses.end())
		? ReadPtrPath(c_addressId) : m_cachedAddresses[c_addressId];
	return m_process->readInt16(addr);
}

int32_t GameData::ReadInt32(const char* c_addressId)
{
	gameAddr addr = (m_cachedAddresses.find(c_addressId) == m_cachedAddresses.end())
		? ReadPtrPath(c_addressId) : m_cachedAddresses[c_addressId];
	return m_process->readInt32(addr);
}

int64_t GameData::ReadInt64(const char* c_addressId)
{
	gameAddr addr = (m_cachedAddresses.find(c_addressId) == m_cachedAddresses.end())
				? ReadPtrPath(c_addressId) : m_cachedAddresses[c_addressId];
	return m_process->readInt64(addr);
}

gameAddr GameData::ReadPtr(const char* c_addressId)
{
	return (gameAddr)ReadInt64(c_addressId);
}

float GameData::ReadFloat(const char* c_addressId)
{
	gameAddr addr = (m_cachedAddresses.find(c_addressId) == m_cachedAddresses.end())
		? ReadPtrPath(c_addressId) : m_cachedAddresses[c_addressId];
	return m_process->readFloat(addr);
}

void GameData::ReadBytes(const char* c_addressId, void* buf, size_t readSize)
{
	gameAddr addr = (m_cachedAddresses.find(c_addressId) == m_cachedAddresses.end())
		? ReadPtrPath(c_addressId) : m_cachedAddresses[c_addressId];
	return m_process->readBytes(addr, buf, readSize);
}