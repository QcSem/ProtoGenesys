#pragma once
#include "../Engine/Engine.hpp"
enum x86Instruction : BYTE { CALL = 0xE8, JMP };

union Ptr
{
	void*  void_ptr;
	void** void_double_ptr;

	BYTE*  byte_ptr;
	WORD*  word_ptr;
	DWORD* dword_ptr;
};

class FurtiveHook
{
public:
	FurtiveHook(x86Instruction instruction, void* address, void* target_function, unsigned int nops = 0, bool unhook_in_dtor = false);
	~FurtiveHook();

	void SetHook();
	void UnHook();

	bool IsHooked() const;

private:
	x86Instruction	  instruction;
	Ptr				  address;
	Ptr				  target_function;
	unsigned int	  nops;
	bool			  unhook_in_dtor;
	bool			  hooked;
	std::vector<BYTE> old_bytes;

	FurtiveHook() = delete;
	FurtiveHook(const FurtiveHook& other) = delete;
	FurtiveHook(FurtiveHook&& other) = delete;

	FurtiveHook& operator=(const FurtiveHook& other) = delete;
	FurtiveHook& operator=(FurtiveHook&& other) = delete;
};


class HotPatch
{
public:
	HotPatch(void* function, void* new_function, bool unpatch_in_dtor = false);
	~HotPatch();

	void* Patch();
	void UnPatch();

	bool IsPatched() const;

private:
	Ptr  function;
	Ptr  new_function;
	bool unpatch_in_dtor;
	bool patched;
#pragma pack(push, 1)
	struct HotPatchData
	{
		BYTE  JMP;
		void* function;
		WORD  JMP_back;
	} *hotpatch;
#pragma pack(pop)

	HotPatch() = delete;
	HotPatch(const HotPatch& other) = delete;
	HotPatch(HotPatch&& other) = delete;

	HotPatch& operator=(const HotPatch& other) = delete;
	HotPatch& operator=(HotPatch&& other) = delete;
};