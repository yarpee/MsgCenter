#pragma once

//
// 创建消息中心
// 返回消息中心的key, 不为0时为创建成功
//
DWORD CreateMsgCenter();

//
// 销毁消息中心
// 传入创建消息中心时返回的key, 返回结果
//
BOOL DestroyMsgCenter(DWORD dwKey);
BOOL DestroyMsgCenter();	// 特化版本, 当且仅当只调用CreateMsgCenter一次时

typedef VOID (*CLASS_MEMBER_MSG_FUNC)(VOID* pThis, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef VOID (*COMMON_MSG_FUNC)(UINT uMsg, WPARAM wParam, LPARAM lParam);

#define CLASS_MEMBER_MSG_FUNC_STUB_NAME(classname, funcname) \
	_CLASS_MEMBER_MSG_FUNC_STUB_##classname##_##funcname

#define COMMON_MSG_FUNC_STUB_NAME(funcname) \
	_COMMON_MSG_FUNC_STUB_##funcname

#define CLASS_MEMBER_MSG_FUNC_IMPL(classname, funcname) \
	static VOID CLASS_MEMBER_MSG_FUNC_STUB_NAME(classname, funcname)(VOID* pThis, UINT uMsg, WPARAM wParam, LPARAM lParam) \
	{ \
		((classname*)pThis)->funcname(uMsg, wParam, lParam); \
	}

#define COMMON_MSG_FUNC_IMPL(funcname) \
	static VOID COMMON_MSG_FUNC_STUB_NAME(funcname)(UINT uMsg, WPARAM wParam, LPARAM lParam) \
	{ \
		funcname(uMsg, wParam, lParam); \
	}

#define REGISTER_CLASS_MEMBER_MSG_FUNC_1(classname, pThis, uMsg, funcname) \
	RegisterClassMemberMsgFunc(pThis, uMsg, classname::CLASS_MEMBER_MSG_FUNC_STUB_NAME(classname, funcname));

#define UNREGISTER_CLASS_MEMBER_MSG_FUNC_1(classname, pThis, uMsg, funcname) \
	UnregisterClassMemberMsgFunc(pThis, uMsg, classname::CLASS_MEMBER_MSG_FUNC_STUB_NAME(classname, funcname));

#define REGISTER_COMMON_MSG_FUNC_1(uMsg, funcname) \
	RegisterCommonMsgFunc(uMsg, COMMON_MSG_FUNC_STUB_NAME(funcname));

#define UNREGISTER_COMMON_MSG_FUNC_1(uMsg, funcname) \
	UnregisterCommonMsgFunc(uMsg, COMMON_MSG_FUNC_STUB_NAME(funcname));

// 注册消息处理的类成员函数
VOID RegisterClassMemberMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);
VOID RegisterClassMemberMsgFunc(VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);

// 注册消息处理的非类成员函数
VOID RegisterCommonMsgFunc(DWORD dwKey, UINT uMsg, COMMON_MSG_FUNC pFunc);
VOID RegisterCommonMsgFunc(UINT uMsg, COMMON_MSG_FUNC pFunc);

// 反注册消息处理的类成员函数
VOID UnregisterClassMemberMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);
VOID UnregisterClassMemberMsgFunc(VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);

// 反注册消息处理的非类成员函数
VOID UnregisterCommonMsgFunc(DWORD dwKey, UINT uMsg, COMMON_MSG_FUNC pFunc);
VOID UnregisterCommonMsgFunc(UINT uMsg, COMMON_MSG_FUNC pFunc);

VOID SendMessage(DWORD dwKey, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID PostMessage(DWORD dwKey, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
