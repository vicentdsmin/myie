#include "stdafx.h"
#include "PerfMon.h"
#include "rasSpdM.h"


BOOL glo_fFromDynReg=TRUE, glo_fSuc = FALSE, glo_fInitialized = FALSE;
BOOL glo_fInitializing = FALSE;
CPerfMon m_PerfMon;
int m_nRAS;

/*
��9x��ͨ��dyn_dataע����õ��ٶ�
*/
DWORD GetTotalRecv(void)
{
	DWORD dwValue=(DWORD)-999;
//	try{

	HKEY hKey;
	if(0==RegOpenKey(HKEY_DYN_DATA,"PerfStats\\StatData",&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,"Dial-Up Adapter\\TotalBytesRecvd",NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}

//	}catch(...)
//	{
//	}
	return dwValue;
}
/*
DWORD GetTotalXmit(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,"PerfStats\\StatData",&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,"Dial-Up Adapter\\TotalBytesXmit",NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}
DWORD GetPerSecondRecv(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,"PerfStats\\StatData",&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,"Dial-Up Adapter\\BytesRecvd",NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}
DWORD GetPerSecondXmit(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,"PerfStats\\StatData",&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,"Dial-Up Adapter\\BytesXmit",NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}*/

/*****************************************************************
��NT��ͨ��per Data�õ�
 *****************************************************************/



/*****************************************************************
�õ���RAS�ٶ�
 *****************************************************************/
inline void GetRASSpeedOn9X(/*DWORD* pTPS,DWORD* pRPS,DWORD* pTT,*/ DWORD* pRT)
{// under 9x
//	*pTPS=GetPerSecondXmit();
//	*pRPS=GetPerSecondRecv();
//	*pTT=GetTotalXmit();
	*pRT=GetTotalRecv();
}

inline void GetRASSpeedOnNT(/*DWORD* pTPS,DWORD* pRPS,DWORD* pTT,*/ DWORD* pRT)
{// under NT
////������ÿ�뷢�ͣ�ÿ����գ��ܷ��ͣ��ܽ���
	try{

	if(glo_fSuc)
	{
		if (!m_PerfMon.CollectQueryData())
		{
			*pRT = (DWORD)-999;
			//AfxMessageBox("Failed Query!");
			return;
		}
		*pRT = m_PerfMon.GetCounterValue(m_nRAS);
	}
	else
		*pRT = (DWORD)-999;

	}catch(...)
	{
	}
}

void GetRASSpeed(/*DWORD* pTPS,DWORD* pRPS,DWORD* pTT,*/ DWORD* pRT)
{////������ÿ�뷢�ͣ�ÿ����գ��ܷ��ͣ��ܽ���
	if(glo_fFromDynReg==FALSE )
	{
		if(glo_fSuc)
			GetRASSpeedOnNT(/*pTPS,pRPS, pTT,*/ pRT);
		else
			*pRT = -999;
	}
	else
	{
		GetRASSpeedOn9X(/*pTPS,pRPS, pTT,*/ pRT);
	}
}

void InitSpeedMonitor(void)
{
	if(glo_fInitialized || glo_fInitializing)
		return;

	glo_fInitializing = TRUE;

	DWORD dwVersion = GetVersion();
	// Get major and minor version numbers of Windows
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	// Get build numbers for Windows NT or Win32s
	if (dwVersion>>31 ==0)                // Windows NT
	{
		glo_fFromDynReg=FALSE;
		//add conter here
		if (!m_PerfMon.Initialize())
		{
			glo_fFromDynReg = TRUE;
		}
		else
		{
			m_nRAS = m_PerfMon.AddCounter(CNTR_RAS_RECEIVBYTES);
			if(m_nRAS==-1)
				glo_fSuc = FALSE;
			else
				glo_fSuc = TRUE;
		}

	}
	else         // Windows 95 - 98
		glo_fFromDynReg=TRUE;

	glo_fInitialized = TRUE;
}

void ClearSpeedMonitor(void)
{
	if(!glo_fFromDynReg && glo_fInitialized)
	{
		if(glo_fSuc)
			m_PerfMon.RemoveCounter(0);
		glo_fSuc = FALSE;
		m_PerfMon.Uninitialize();
	}
	glo_fInitialized = FALSE;
}

