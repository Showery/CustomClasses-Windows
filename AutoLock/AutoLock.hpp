#pragma once
#include <WinBase.h>
// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    void operator=(const TypeName&)


//�ٽ�����װ
class SectionLock {
private:
    // make copy constructor and assignment operator inaccessible
    DISALLOW_COPY_AND_ASSIGN(SectionLock);
    CRITICAL_SECTION _critical_section;

public:
    SectionLock() {
        InitializeCriticalSection(&_critical_section);
    };

    ~SectionLock() {
        DeleteCriticalSection(&_critical_section);
    };

    void Lock() {
        EnterCriticalSection(&_critical_section);
    };

    void Unlock() {
        LeaveCriticalSection(&_critical_section);
    };
};
//ʹ�÷�װ�ٽ������Զ���
class AutoLock {
private:
    // make copy constructor and assignment operator inaccessible
    DISALLOW_COPY_AND_ASSIGN(AutoLock);

protected:
    SectionLock * _section_lock;

public:
    AutoLock(SectionLock * plock) {
        _section_lock = plock;
        _section_lock->Lock();
    };

    ~AutoLock() {
        _section_lock->Unlock();
    };
};