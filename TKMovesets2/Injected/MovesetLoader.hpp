#pragma once

#include <windows.h>
#include <stdio.h>
#include <map>
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/ZydisDisassembler.hpp>

#include "constants.h"
#include "SharedMemory.h"
#include "InjectionUtils.hpp"

typedef unsigned char Byte;

struct functionHook
{
	PLH::x64Detour* detour;
	uint64_t trampoline;
};

class MovesetLoader
{
protected:
	// Stores a handle to the shared memory
	HANDLE m_memoryHandle = nullptr;
	// Ptr to the shared memory
	SharedMemory* m_sharedMemPtr = nullptr;
	// List of hooked functions
	std::map<std::string, functionHook> m_hooks;
	// Disassembler that must be passed to any hook creation
	PLH::ZydisDisassembler m_disassembler = PLH::ZydisDisassembler(PLH::Mode::x64);
	// Contains our own module address
	uint64_t m_moduleAddr;
	// Contains the module name
	std::string m_moduleName;

	// Returns the name of the shared memory to look after
	virtual const TCHAR* GetSharedMemoryName() = 0;
public:

	// Initializes the shared memory
	bool Init();
	~MovesetLoader();

	// Cast a trampoline function and returns it
	template<class T> T CastTrampoline(std::string hookName);
	// Initializes a hook for a game function and returns a pointer to its detour (call ->hook() to actually hook)
	PLH::x64Detour* InitHook(const char* hookName, uint64_t originalAddr, uint64_t newAddr);

	// Called once shared memory has been successfully loaded
	virtual void PostInit() = 0;
	// If set to true, force the Mainloop() to stop
	bool mustStop = false;
	// Main loop of the loader
	void Mainloop();
	// Returns true if shared memory file has been successfully initialized
	bool isInitialized() { return m_memoryHandle != nullptr; }
	// Sets the main module name and address
	void SetMainModule(const char* name) {
		m_moduleName = name;
		m_moduleAddr = InjectionUtils::GetSelfModuleAddress(name);
	}
};

template<class T> T MovesetLoader::CastTrampoline(std::string hookName)
{
	if (m_hooks.contains(hookName)) {
		return (T)m_hooks[hookName].trampoline;
	}
	DEBUG_LOG("CastTrampoline(): Hook '%s' not found\n", hookName.c_str());
	return nullptr;
}