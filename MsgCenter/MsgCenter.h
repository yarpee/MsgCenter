#pragma once

//
// ������Ϣ����
// ������Ϣ���ĵ�key, ��Ϊ0ʱΪ�����ɹ�
//
DWORD CreateMsgCenter();

//
// ������Ϣ����
// ���봴����Ϣ����ʱ���ص�key, ���ؽ��
//
BOOL DestroyMsgCenter(DWORD dwKey);
BOOL DestroyMsgCenter();	// �ػ��汾, ���ҽ���ֻ����CreateMsgCenterһ��ʱ

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

// ע����Ϣ��������Ա����
VOID RegisterClassMemberMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);
VOID RegisterClassMemberMsgFunc(VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);

// ע����Ϣ����ķ����Ա����
VOID RegisterCommonMsgFunc(DWORD dwKey, UINT uMsg, COMMON_MSG_FUNC pFunc);
VOID RegisterCommonMsgFunc(UINT uMsg, COMMON_MSG_FUNC pFunc);

// ��ע����Ϣ��������Ա����
VOID UnregisterClassMemberMsgFunc(DWORD dwKey, VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);
VOID UnregisterClassMemberMsgFunc(VOID* pThis, UINT uMsg, CLASS_MEMBER_MSG_FUNC pFunc);

// ��ע����Ϣ����ķ����Ա����
VOID UnregisterCommonMsgFunc(DWORD dwKey, UINT uMsg, COMMON_MSG_FUNC pFunc);
VOID UnregisterCommonMsgFunc(UINT uMsg, COMMON_MSG_FUNC pFunc);

VOID SendMessage(DWORD dwKey, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID PostMessage(DWORD dwKey, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
