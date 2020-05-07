//=====================================================================================

#pragma once
#include <stdint.h>
#include <winnt.h>

//=====================================================================================

#if defined (_M_AMD64) || defined (__x86_64__)
#define USE64
#elif defined (_M_IX86) || defined (__i386__)
#define USE32
#endif

//=====================================================================================

#pragma pack(push, 1)
struct ThunkData
{
#ifndef USE64
    uint8_t  mov1 = 0xB8;
    void* pInst = nullptr;
    uint16_t fs1 = '\x64\xA3';
    uint8_t  fs2 = FIELD_OFFSET(NT_TIB, ArbitraryUserPointer);
    uint8_t  fs3 = 0;
    uint16_t fs4 = 0;
    uint8_t  mov2 = 0xB8;
    void* pFn = nullptr;
    uint16_t jmp1 = '\xFF\xE0';
#else
    uint16_t mov1 = '\x48\xB8';
    void* pInst = nullptr;
    uint32_t fs1 = '\x65\x48\x89\x04';
    uint8_t  fs2 = 0x25;
    uint8_t  fs3 = FIELD_OFFSET(NT_TIB, ArbitraryUserPointer);
    uint8_t  fs4 = 0;
    uint16_t fs5 = 0;
    uint16_t mov2 = '\x48\xB8';
    void* pFn = nullptr;
    uint16_t jmp1 = '\xFF\xE0';
#endif

    void setup(void* pInstance, void* pMethod)
    {
        pInst = pInstance;
        pFn = pMethod;
    }
};
#pragma pack(pop)

//=====================================================================================

template<typename fn, typename C>
class Thunk;

template<typename R, typename... Args, typename C>
class Thunk < R(*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(*)(Args...);

public:
    Thunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &Thunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<Thunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};

//=====================================================================================

template<typename fn, typename C>
class CdeclThunk;

template<typename R, typename... Args, typename C>
class CdeclThunk < R(__cdecl*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(__cdecl*)(Args...);

public:
    CdeclThunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &CdeclThunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R __cdecl WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<CdeclThunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};

//=====================================================================================

#if (_MANAGED == 1) || (_M_CEE == 1)
template<typename fn, typename C>
class ClrThunk;

template<typename R, typename... Args, typename C>
class ClrThunk < R(__clrcall*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(__clrcall*)(Args...);

public:
    ClrThunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &ClrThunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R __clrcall WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<ClrThunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};
#endif

//=====================================================================================

template<typename fn, typename C>
class StdThunk;

template<typename R, typename... Args, typename C>
class StdThunk < R(__stdcall*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(__stdcall*)(Args...);

public:
    StdThunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &StdThunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R __stdcall WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<StdThunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};

//=====================================================================================

template<typename fn, typename C>
class FastThunk;

template<typename R, typename... Args, typename C>
class FastThunk < R(__fastcall*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(__fastcall*)(Args...);

public:
    FastThunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &FastThunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R __fastcall WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<FastThunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};

//=====================================================================================

template<typename fn, typename C>
class ThisThunk;

template<typename R, typename... Args, typename C>
class ThisThunk < R(__thiscall*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(__thiscall*)(Args...);

public:
    ThisThunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &ThisThunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R __thiscall WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<ThisThunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};

//=====================================================================================

template<typename fn, typename C>
class VectorThunk;

template<typename R, typename... Args, typename C>
class VectorThunk < R(__vectorcall*)(Args...), C >
{
public:
    using TypeMember = R(C::*)(Args...);
    using TypeFree = R(__vectorcall*)(Args...);

public:
    VectorThunk(TypeMember pfn, C* pInstance)
        : _pMethod(pfn)
        , _pInstance(pInstance)
    {
        DWORD dwOld = 0;
        VirtualProtect(&_thunk, sizeof(_thunk), PAGE_EXECUTE_READWRITE, &dwOld);
        _thunk.setup(this, &VectorThunk::WrapHandler);
    }
    /*
    //=====================================================================================
    */
    static R __vectorcall WrapHandler(Args... args)
    {
        auto _this = reinterpret_cast<VectorThunk*>(((PNT_TIB)NtCurrentTeb())->ArbitraryUserPointer);
        return (_this->_pInstance->*_this->_pMethod)(args...);
    }
    /*
    //=====================================================================================
    */
    TypeFree GetThunk()
    {
        return reinterpret_cast<TypeFree>(&_thunk);
    }

private:
    TypeMember _pMethod = nullptr;
    C* _pInstance = nullptr;
    ThunkData _thunk;
};

//=====================================================================================