//=====================================================================================

#pragma once
#include "../Engine/Engine.hpp"

class CVMTHookManager
{
public:
	CVMTHookManager(void);
	CVMTHookManager(PDWORD* ppdwClassBase);
	~CVMTHookManager(void);

	bool bInitialize(PDWORD* ppdwClassBase);
	bool bInitialize(PDWORD** pppdwClassBase);

	void UnHook(void);
	void ReHook(void);

	int iGetFuncCount(void);

	DWORD dwGetMethodAddress(int Index);
	PDWORD pdwGetOldVMT(void);
	DWORD dwHookMethod(DWORD dwNewFunc, unsigned int iIndex);

private:
	DWORD dwGetVMTCount(PDWORD pdwVMT);
	PDWORD* m_ppdwClassBase;
	PDWORD	m_pdwNewVMT, m_pdwOldVMT;
	DWORD	m_dwVMTSize, oFunction;
	int unhookindex;
};

//=====================================================================================