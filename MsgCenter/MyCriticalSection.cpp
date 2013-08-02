#include "StdAfx.h"
#include "MyCriticalSection.h"

CMyCriticalSection::CMyCriticalSection(void)
{
	InitializeCriticalSection(&m_cs);
}

CMyCriticalSection::~CMyCriticalSection(void)
{
	DeleteCriticalSection(&m_cs);
}

VOID CMyCriticalSection::Lock()
{
	EnterCriticalSection(&m_cs);
}

VOID CMyCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_cs);
}

CCriticalSectionLock::CCriticalSectionLock(CMyCriticalSection& cs)
: m_cs(cs)
{
	m_cs.Lock();
}

CCriticalSectionLock::~CCriticalSectionLock()
{
	m_cs.Unlock();
}
