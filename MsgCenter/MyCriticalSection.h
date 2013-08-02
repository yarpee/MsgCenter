#pragma once

class CMyCriticalSection
{
public:
	CMyCriticalSection(void);
	~CMyCriticalSection(void);

	VOID Lock();
	VOID Unlock();

private:
	CRITICAL_SECTION m_cs;
};

class CCriticalSectionLock
{
public:
	CCriticalSectionLock(CMyCriticalSection& cs);
	~CCriticalSectionLock();

private:
	CMyCriticalSection& m_cs;
};
