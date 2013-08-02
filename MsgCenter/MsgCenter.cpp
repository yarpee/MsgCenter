#include "stdafx.h"
#include "MsgCenter.h"
#include "MyCriticalSection.h"
#include <list>
#include <map>
using namespace std;

#define MSG_WINDOW_CLASS_NAME	_T("MsgCenterClass")
#define MSG_WINDOW_NAME_FORMAT	_T("MsgWnd%lu")
#define MSG_WINDOW_NAME_LENGTH	20

typedef struct _MSG_FUNC
{
	VOID* pThis;
	VOID* pFunc;
} MSG_FUNC, *PMSG_FUNC;

typedef list<MSG_FUNC> MSG_FUNC_LIST;
typedef map<UINT, MSG_FUNC_LIST> MSG_FUNC_MAP;

CMyCriticalSection g_cs;
DWORD g_dwMsgWndIndex = 0;
map<DWORD, HWND> g_MsgWndMap;
map<HWND, MSG_FUNC_MAP> g_MsgWndFuncMap;

typedef list<MSG_FUNC>::iterator MSG_FUNC_LIST_IT;
typedef map<UINT, MSG_FUNC_LIST>::iterator MSG_FUNC_MAP_IT;
typedef map<HWND, MSG_FUNC_MAP>::iterator MSG_WND_FUNC_MAP_IT;

LRESULT CALLBACK MsgCenterProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCriticalSectionLock Lock(g_cs);
	MSG_WND_FUNC_MAP_IT itWnd = g_MsgWndFuncMap.find(hwnd);
	if(itWnd != g_MsgWndFuncMap.end())
	{
		MSG_FUNC_MAP_IT itMsg = (itWnd->second).find(uMsg);
		if(itMsg != (itWnd->second).end())
		{
			for(MSG_FUNC_LIST_IT itFunc = (itMsg->second).begin(); itFunc != (itMsg->second).end(); ++itFunc)
			{
				if(itFunc->pThis != NULL)
				{
					CLASS_MEMBER_MSG_FUNC pFunc = (CLASS_MEMBER_MSG_FUNC)(itFunc->pFunc);
					pFunc(itFunc->pThis, uMsg, wParam, lParam);
				}
				else
				{
					COMMON_MSG_FUNC pFunc = (COMMON_MSG_FUNC)(itFunc->pFunc);
					pFunc(uMsg, wParam, lParam);
				}
			}
		}
	}
	return TRUE;
}

DWORD CreateMsgCenter()
{
	CCriticalSectionLock Lock(g_cs);
	DWORD dwKey = ++g_dwMsgWndIndex;
	TCHAR szMsgWndName[MSG_WINDOW_NAME_LENGTH] = {0};
	wsprintf(szMsgWndName, MSG_WINDOW_NAME_FORMAT, dwKey);

	WNDCLASS wc = {0};
	wc.lpfnWndProc = MsgCenterProc;
	wc.hInstance = GetModuleHandle(NULL);	// exe's handle
	wc.lpszClassName = MSG_WINDOW_CLASS_NAME;
	RegisterClass(&wc);	// 如果返回0也有可能是该窗口类已经注册了, 如果先使用GetClassInfo做判断又有可能多线程原因返回值不正确
	HWND hWnd = CreateWindow(MSG_WINDOW_CLASS_NAME, szMsgWndName, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
	if(hWnd != NULL)
	{
		g_MsgWndMap[dwKey] = hWnd;
		return dwKey;
	}
	return 0;
}

BOOL DestroyMsgCenter(DWORD dwKey)
{
	BOOL bRet = FALSE;
	CCriticalSectionLock Lock(g_cs);
	HWND hWnd = g_MsgWndMap[dwKey];
	if(hWnd != NULL)
	{
		bRet = DestroyWindow(hWnd);
		if(bRet)
		{
			g_MsgWndMap.erase(dwKey);
			if(0 == g_MsgWndMap.size())
			{
				UnregisterClass(MSG_WINDOW_CLASS_NAME, GetModuleHandle(NULL));
			}
		}
	}
	return bRet;
}

BOOL DestroyMsgCenter()
{
	CCriticalSectionLock Lock(g_cs);
	return DestroyMsgCenter(g_dwMsgWndIndex);
}

VOID RegisterMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, VOID* pFunc)
{
	if(dwKey != 0 && pFunc != NULL)
	{
		CCriticalSectionLock Lock(g_cs);
		HWND hWnd = g_MsgWndMap[dwKey];
		if(hWnd != NULL)
		{
			MSG_WND_FUNC_MAP_IT itWnd = g_MsgWndFuncMap.find(hWnd);
			if(itWnd != g_MsgWndFuncMap.end())
			{
				MSG_FUNC_MAP_IT itMsg = (itWnd->second).find(uMsg);
				if(itMsg != (itWnd->second).end())
				{
					MSG_FUNC func = {pThis, pFunc};
					(itMsg->second).push_back(func);
				}
				else
				{
					MSG_FUNC func = {pThis, pFunc};
					MSG_FUNC_LIST list;
					list.push_back(func);
					MSG_FUNC_MAP map;
					map[uMsg] = list;
					itWnd->second = map;
				}
			}
			else
			{
				MSG_FUNC func = {pThis, pFunc};
				MSG_FUNC_LIST list;
				list.push_back(func);
				MSG_FUNC_MAP map;
				map[uMsg] = list;
				g_MsgWndFuncMap[hWnd] = map;
			}
		}
	}
}

VOID UnregisterMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, VOID* pFunc)
{
	if(dwKey != 0 && pFunc != NULL)
	{
		CCriticalSectionLock Lock(g_cs);
		HWND hWnd = g_MsgWndMap[dwKey];
		if(hWnd != NULL)
		{
			MSG_WND_FUNC_MAP_IT itWnd = g_MsgWndFuncMap.find(hWnd);
			if(itWnd != g_MsgWndFuncMap.end())
			{
				MSG_FUNC_MAP_IT itMsg = (itWnd->second).find(uMsg);
				if(itMsg != (itWnd->second).end())
				{
					for(MSG_FUNC_LIST_IT itFunc = (itMsg->second).begin(); itFunc != (itMsg->second).end(); ++itFunc)
					{
						if(itFunc->pThis == pThis && itFunc->pFunc == pFunc)
						{
							(itMsg->second).erase(itFunc);
							break;
						}
					}
					if((itMsg->second).size() == 0)
					{
						(itWnd->second).erase(itMsg);
					}
					if((itWnd->second).size() == 0)
					{
						g_MsgWndFuncMap.erase(itWnd);
					}
				}
			}
		}
	}
}

VOID RegisterClassMemberMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc)
{
	return RegisterMsgFunc(dwKey, pThis, uMsg, pFunc);
}

VOID RegisterClassMemberMsgFunc(VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc)
{
	CCriticalSectionLock Lock(g_cs);
	return RegisterClassMemberMsgFunc(g_dwMsgWndIndex, pThis, uMsg, pFunc);
}

VOID RegisterCommonMsgFunc(DWORD dwKey, UINT uMsg, COMMON_MSG_FUNC pFunc)
{
	return RegisterMsgFunc(dwKey, NULL, uMsg, pFunc);
}

VOID RegisterCommonMsgFunc(UINT uMsg, COMMON_MSG_FUNC pFunc)
{
	CCriticalSectionLock Lock(g_cs);
	return RegisterCommonMsgFunc(g_dwMsgWndIndex, uMsg, pFunc);
}

VOID UnregisterClassMemberMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc)
{
	return UnregisterMsgFunc(dwKey, pThis, uMsg, pFunc);
}

VOID UnregisterClassMemberMsgFunc(VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc)
{
	CCriticalSectionLock Lock(g_cs);
	return UnregisterClassMemberMsgFunc(g_dwMsgWndIndex, pThis, uMsg, pFunc);
}

VOID UnregisterCommonMsgFunc(DWORD dwKey, UINT uMsg, COMMON_MSG_FUNC pFunc)
{
	return UnregisterMsgFunc(dwKey, NULL, uMsg, pFunc);
}

VOID UnregisterCommonMsgFunc(UINT uMsg, COMMON_MSG_FUNC pFunc)
{
	CCriticalSectionLock Lock(g_cs);
	return UnregisterCommonMsgFunc(g_dwMsgWndIndex, uMsg, pFunc);
}

VOID SendMessage(DWORD dwKey, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(dwKey != 0)
	{
		HWND hWnd = NULL;
		{
			CCriticalSectionLock Lock(g_cs);
			hWnd = g_MsgWndMap[dwKey];
		}
		::SendMessage(hWnd, uMsg, wParam, lParam);
	}
}

VOID SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD dwKey = 0;
	{
		CCriticalSectionLock Lock(g_cs);
		dwKey = g_dwMsgWndIndex;
	}
	return SendMessage(dwKey, uMsg, wParam, lParam);
}

VOID PostMessage(DWORD dwKey, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(dwKey != 0)
	{
		HWND hWnd = NULL;
		{
			CCriticalSectionLock Lock(g_cs);
			hWnd = g_MsgWndMap[dwKey];
		}
		::PostMessage(hWnd, uMsg, wParam, lParam);
	}
}

VOID PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD dwKey = 0;
	{
		CCriticalSectionLock Lock(g_cs);
		dwKey = g_dwMsgWndIndex;
	}
	return PostMessage(dwKey, uMsg, wParam, lParam);
}
