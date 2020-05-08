//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

FurtiveHook::~FurtiveHook()
{
	if (unhook_in_dtor && hooked)
	{
		try
		{
			UnHook();
		}
		catch (...)
		{

		}
	}
}

//=====================================================================================

void FurtiveHook::SetHook()
{
	DWORD old_protection{ 0 };

	VirtualProtect(address.void_ptr, 5 + nops, PAGE_EXECUTE_READWRITE, &old_protection);

	for (unsigned int i = 0; i < 5 + nops; ++i)
	{
		old_bytes.push_back(address.byte_ptr[i]);
	}

	*address.byte_ptr = instruction;

	DWORD call_bytes{ static_cast<DWORD>(target_function.byte_ptr - (address.byte_ptr + 5)) };

	*reinterpret_cast<DWORD*>(address.byte_ptr + 1) = call_bytes;

	for (unsigned int i = 5; i < (5 + nops); ++i)
	{
		address.byte_ptr[i] = 0x90;
	}

	hooked = true;

	VirtualProtect(address.void_ptr, 5 + nops, DWORD(old_protection), &old_protection);
}

//=====================================================================================

void FurtiveHook::UnHook()
{
	DWORD old_protection{ 0 };

	VirtualProtect(address.void_ptr, 5 + nops, PAGE_EXECUTE_READWRITE, &old_protection);

	for (unsigned int i = 0; i < 5 + nops; ++i)
	{
		address.byte_ptr[i] = old_bytes[i];
	}

	hooked = false;

	VirtualProtect(address.void_ptr, 5 + nops, DWORD(old_protection), &old_protection);
}

//=====================================================================================

bool FurtiveHook::IsHooked() const
{
	return hooked;
}

//=====================================================================================

FurtiveHook::FurtiveHook(x86Instruction instruction, void* address, void* target_function, unsigned int nops, bool unhook_in_dtor) :
	instruction{ instruction },
	address{ address },
	target_function{ target_function },
	nops{ nops },
	unhook_in_dtor{ unhook_in_dtor },
	hooked{ false }
{

}

//=====================================================================================

HotPatch::HotPatch(void* function, void* new_function, bool unpatch_in_dtor) :
	function{ function },
	new_function{ new_function },
	unpatch_in_dtor{ unpatch_in_dtor },
	patched{ false }
{

}

//=====================================================================================

HotPatch::~HotPatch()
{
	if (unpatch_in_dtor && patched)
	{
		try
		{
			UnPatch();
		}
		catch (...)
		{

		}
	}
}

//=====================================================================================

void* HotPatch::Patch()
{
	void* original_function = nullptr;

	DWORD protection{ 0 };

	hotpatch = reinterpret_cast<HotPatchData*>(function.byte_ptr - 5);

	VirtualProtect(hotpatch, 7, PAGE_EXECUTE_READWRITE, &protection);

	original_function = function.byte_ptr + 2;
	hotpatch->JMP = 0xE9;
	hotpatch->function = reinterpret_cast<void*>(new_function.byte_ptr - function.byte_ptr);
	hotpatch->JMP_back = 0xF9EB;

	patched = true;

	VirtualProtect(hotpatch, 7, protection, &protection);

	return original_function;
}

//=====================================================================================

void HotPatch::UnPatch()
{
	DWORD protection{ 0 };

	VirtualProtect(hotpatch, 7, PAGE_EXECUTE_READWRITE, &protection);

	hotpatch->JMP_back = 0xFF8B;
	hotpatch->JMP = 0x90;
	hotpatch->function = reinterpret_cast<void*>(0x90909090);

	patched = false;

	VirtualProtect(hotpatch, 7, protection, &protection);
}

//=====================================================================================

bool HotPatch::IsPatched() const
{
	return patched;
}

//=====================================================================================