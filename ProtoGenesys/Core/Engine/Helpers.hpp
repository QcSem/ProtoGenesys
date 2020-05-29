//=====================================================================================

#pragma once

//=====================================================================================

namespace ProtoGenesys
{
	template<typename Return, typename... Parameters>
	FORCEINLINE Return VariadicCall(DWORD_PTR address, Parameters... params)
	{
		typedef Return(*tFunction)(Parameters...);
		tFunction Function = (tFunction)address;
		return Function(params...);
	}
	/*
	//=====================================================================================
	*/
	template<typename... Parameters>
	FORCEINLINE std::string VariadicText(LPCSTR format, Parameters... params)
	{
		char szBuffer[4096] = { NULL };
		sprintf_s(szBuffer, format, params...);
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
	FORCEINLINE bool Match(const BYTE* data, const BYTE* signature, const char* mask)
	{
		for (; *mask; ++mask, ++data, ++signature)
			if (*mask == 'x' && *data != *signature)
				return false;

		return !(*mask);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD_PTR FindPattern(const char* signature, const char* mask)
	{
		DWORD_PTR dwAddress = (DWORD_PTR)GetModuleHandle(NULL);
		DWORD_PTR dwLen = GetModuleInfo(NULL).SizeOfImage;

		for (DWORD_PTR i = 0; i < dwLen; i++)
			if (Match((BYTE*)(dwAddress + i), (BYTE*)signature, mask))
				return (DWORD_PTR)(dwAddress + i);

		return NULL;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD_PTR FindPattern(DWORD_PTR address, DWORD_PTR length, const char* signature, const char* mask)
	{
		for (DWORD_PTR i = 0; i < length; i++)
			if (Match((BYTE*)(address + i), (BYTE*)signature, mask))
				return (DWORD_PTR)(address + i);

		return NULL;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD_PTR ReadPointer(DWORD_PTR address, DWORD_PTR offset)
	{
		if (!address)
			return 0;

		if (sizeof(LPVOID) == 0x8)
			return address + (int)((*(int*)(address + offset) + offset) + sizeof(int));

		return *(DWORD_PTR*)(address + offset);
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD_PTR FindDmaAddy(DWORD_PTR address, std::vector<DWORD_PTR> offsets)
	{
		DWORD_PTR dwPointer = *(DWORD_PTR*)address;

		if (!dwPointer)
			return NULL;

		for (auto Offset : offsets)
		{
			if (Offset == offsets.back())
			{
				dwPointer = (DWORD_PTR)(dwPointer + Offset);

				if (!dwPointer)
					return NULL;

				return dwPointer;
			}

			else
			{
				dwPointer = *(DWORD_PTR*)(dwPointer + Offset);

				if (!dwPointer)
					return NULL;
			}
		}

		return dwPointer;
	}
	/*
	//=====================================================================================
	*/
	FORCEINLINE DWORD_PTR SwapVMT(DWORD_PTR address, DWORD_PTR hook, int index)
	{
		DWORD_PTR* dwVTable = *(DWORD_PTR**)address;
		DWORD_PTR dwBackup = NULL;

		MEMORY_BASIC_INFORMATION MBI;

		VirtualQuery((LPCVOID)dwVTable, &MBI, sizeof(MEMORY_BASIC_INFORMATION));
		VirtualProtect(MBI.BaseAddress, MBI.RegionSize, PAGE_EXECUTE_READWRITE, &MBI.Protect);

		dwBackup = dwVTable[index];
		dwVTable[index] = (DWORD_PTR)hook;

		VirtualProtect(MBI.BaseAddress, MBI.RegionSize, MBI.Protect, &MBI.Protect);

		return dwBackup;
	}
}

//=====================================================================================