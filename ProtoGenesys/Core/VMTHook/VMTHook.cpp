//=====================================================================================

#include "../../StdAfx.hpp"

//=====================================================================================

CVMTHookManager::CVMTHookManager(void)
{
	memset(this, 0, sizeof(CVMTHookManager));
}

//=====================================================================================

CVMTHookManager::CVMTHookManager(PDWORD* ppdwClassBase)
{
	bInitialize(ppdwClassBase);
}

//=====================================================================================

CVMTHookManager::~CVMTHookManager(void)
{
	UnHook();
}

//=====================================================================================

bool CVMTHookManager::bInitialize(PDWORD* ppdwClassBase)
{
	m_ppdwClassBase = ppdwClassBase;
	m_pdwOldVMT = *ppdwClassBase;
	m_dwVMTSize = dwGetVMTCount(*ppdwClassBase);
	m_pdwNewVMT = new DWORD[m_dwVMTSize];
	memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(DWORD) * m_dwVMTSize);
	*ppdwClassBase = m_pdwNewVMT;
	return true;
}

//=====================================================================================

bool CVMTHookManager::bInitialize(PDWORD** pppdwClassBase)
{
	return bInitialize(*pppdwClassBase);
}

//=====================================================================================

void CVMTHookManager::UnHook(void)
{
	dwHookMethod(oFunction, unhookindex);
}

//=====================================================================================

void CVMTHookManager::ReHook(void)
{
	if (m_ppdwClassBase)
	{
		*m_ppdwClassBase = m_pdwNewVMT;
	}
}

//=====================================================================================

int CVMTHookManager::iGetFuncCount(void)
{
	return (int)m_dwVMTSize;
}

//=====================================================================================

DWORD CVMTHookManager::dwGetMethodAddress(int Index)
{
	if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
	{
		unhookindex = Index;
		oFunction = m_pdwOldVMT[Index];
		return m_pdwOldVMT[Index];
	}
	return NULL;
}

//=====================================================================================

PDWORD CVMTHookManager::pdwGetOldVMT(void)
{
	return m_pdwOldVMT;
}

//=====================================================================================

DWORD CVMTHookManager::dwHookMethod(DWORD dwNewFunc, unsigned int iIndex)
{
	if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
	{
		m_pdwNewVMT[iIndex] = dwNewFunc;
		return m_pdwOldVMT[iIndex];
	}

	return NULL;
}

//=====================================================================================

DWORD CVMTHookManager::dwGetVMTCount(PDWORD pdwVMT)
{
	DWORD dwIndex = 0;

	for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
	{
		if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
		{
			break;
		}
	}
	return dwIndex;
}

//=====================================================================================