//=====================================================================================

#pragma once

//=====================================================================================

namespace ProtoGenesys
{
	template<typename Return, typename... Parameters>
	FORCEINLINE Return VariadicCall(std::uintptr_t address, Parameters... params)
	{
		typedef Return(*tFunction)(Parameters...);
		tFunction Function = (tFunction)address;
		return Function(params...);
	}
	/*
	//=====================================================================================
	*/
	template<typename... Parameters>
	FORCEINLINE std::string VariadicText(std::string format, Parameters... params)
	{
		char szBuffer[4096] = { NULL };
		sprintf_s(szBuffer, format.c_str(), params...);
		return szBuffer;
	}
	/*
	//=====================================================================================
	*/
	template<typename Type>
	FORCEINLINE void WriteMemoryProtected(LPVOID address, Type value)
	{
		DWORD dwProtection = PAGE_EXECUTE_READWRITE;

		VirtualProtect(address, sizeof(Type), dwProtection, &dwProtection);
		*(Type*)address = value;
		VirtualProtect(address, sizeof(Type), dwProtection, &dwProtection);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE MODULEINFO GetModuleInfo(LPCSTR name)
	{
		MODULEINFO ModuleInfo = { NULL };
		HMODULE hModule = GetModuleHandle(name);

		if (!hModule)
			return ModuleInfo;

		GetModuleInformation(GetCurrentProcess(), hModule, &ModuleInfo, sizeof(MODULEINFO));
		return ModuleInfo;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE bool Match(LPBYTE data, LPBYTE signature, LPCSTR mask)
	{
		for (; *mask; ++mask, ++data, ++signature)
			if (*mask == 'x' && *data != *signature)
				return false;

		return !(*mask);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE std::uintptr_t FindPattern(LPCSTR signature, LPCSTR mask)
	{
		std::uintptr_t dwAddress = (std::uintptr_t)GetModuleInfo(NULL).lpBaseOfDll;
		std::uintptr_t dwLen = GetModuleInfo(NULL).SizeOfImage;

		for (std::uintptr_t i = 0; i < dwLen; i++)
			if (Match((LPBYTE)(dwAddress + i), (LPBYTE)signature, mask))
				return (std::uintptr_t)(dwAddress + i);

		return NULL;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE std::uintptr_t FindPattern(std::uintptr_t address, std::uintptr_t length, LPCSTR signature, LPCSTR mask)
	{
		for (std::uintptr_t i = 0; i < length; i++)
			if (Match((LPBYTE)(address + i), (LPBYTE)signature, mask))
				return (std::uintptr_t)(address + i);

		return NULL;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE std::uintptr_t ReadPointer(std::uintptr_t address, std::uintptr_t offset)
	{
		if (!address)
			return 0;

		if (sizeof(LPVOID) == 0x8)
			return address + (int)((*(int*)(address + offset) + offset) + sizeof(int));

		return *(std::uintptr_t*)(address + offset);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE std::uintptr_t FindDmaAddy(std::uintptr_t address, std::vector<std::uintptr_t> offsets)
	{
		std::uintptr_t dwPointer = *(std::uintptr_t*)address;

		if (!dwPointer)
			return NULL;

		for (auto Offset : offsets)
		{
			if (Offset == offsets.back())
			{
				dwPointer = (std::uintptr_t)(dwPointer + Offset);

				if (!dwPointer)
					return NULL;

				return dwPointer;
			}

			else
			{
				dwPointer = *(std::uintptr_t*)(dwPointer + Offset);

				if (!dwPointer)
					return NULL;
			}
		}

		return dwPointer;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE std::uintptr_t SwapVMT(std::uintptr_t address, std::uintptr_t hook, int index)
	{
		std::uintptr_t* dwVTable = *(std::uintptr_t**)address;
		std::uintptr_t dwBackup = NULL;

		MEMORY_BASIC_INFORMATION MBI;

		VirtualQuery((LPCVOID)dwVTable, &MBI, sizeof(MEMORY_BASIC_INFORMATION));
		VirtualProtect(MBI.BaseAddress, MBI.RegionSize, PAGE_EXECUTE_READWRITE, &MBI.Protect);

		dwBackup = dwVTable[index];
		dwVTable[index] = hook;

		VirtualProtect(MBI.BaseAddress, MBI.RegionSize, MBI.Protect, &MBI.Protect);

		return dwBackup;
	}
}

//=====================================================================================