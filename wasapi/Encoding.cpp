#include "stdafx.h"
#include "Encoding.h"
#include "Win32Exception.h"

using namespace std;

string WideCharToUTF8Impl(const wstring& str, int length) noexcept
{
    string buffer;
    buffer.reserve(length);
    assert(WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &buffer.front(), length, NULL, NULL) > 0);
    return buffer;
}

string WideCharToUTF8(const wstring& str)
{
    int length = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    Check(static_cast<BOOL>(length > 0), __FILE__, __LINE__);
    return WideCharToUTF8Impl(str, length);
}

string WideCharToUTF8NoExcept(const wstring& str) noexcept
{
    int length = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    return length > 0 ? WideCharToUTF8Impl(str, length) : string();
}

wstring UTF8ToWideCharImpl(const string& str, int length) noexcept
{
    wstring buffer;
    buffer.reserve(length);
    assert(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &buffer.front(), length) > 0);
    return buffer;
}

wstring UTF8ToWideChar(const string& str)
{
    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    Check(static_cast<BOOL>(length > 0), __FILE__, __LINE__);
    return UTF8ToWideCharImpl(str, length);
}

wstring UTF8ToWideCharNoExcept(const string& str) noexcept
{
    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    return length > 0 ? UTF8ToWideCharImpl(str, length) : wstring();
}
