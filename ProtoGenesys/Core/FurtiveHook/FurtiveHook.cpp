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

FurtivePatch::FurtivePatch(void* function, void* new_function, bool unpatch_in_dtor) :
	function{ function },
	new_function{ new_function },
	unpatch_in_dtor{ unpatch_in_dtor },
	patched{ false }
{

}

//=====================================================================================

FurtivePatch::~FurtivePatch()
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

void* FurtivePatch::Patch()
{
	void* original_function = nullptr;

	DWORD protection{ 0 };

	furtivepatch = reinterpret_cast<FurtivePatchData*>(function.byte_ptr - 5);

	VirtualProtect(furtivepatch, 7, PAGE_EXECUTE_READWRITE, &protection);

	original_function = function.byte_ptr + 2;
	furtivepatch->JMP = 0xE9;
	furtivepatch->function = reinterpret_cast<void*>(new_function.byte_ptr - function.byte_ptr);
	furtivepatch->JMP_back = 0xF9EB;

	patched = true;

	VirtualProtect(furtivepatch, 7, protection, &protection);

	return original_function;
}

//=====================================================================================

void FurtivePatch::UnPatch()
{
	DWORD protection{ 0 };

	VirtualProtect(furtivepatch, 7, PAGE_EXECUTE_READWRITE, &protection);

	furtivepatch->JMP_back = 0xFF8B;
	furtivepatch->JMP = 0x90;
	furtivepatch->function = reinterpret_cast<void*>(0x90909090);

	patched = false;

	VirtualProtect(furtivepatch, 7, protection, &protection);
}

//=====================================================================================

bool FurtivePatch::IsPatched() const
{
	return patched;
}

//=====================================================================================