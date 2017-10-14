#include "stdafx.h"
#include "COMException.h"
#include "Encoding.h"

using namespace std;

COMException::COMException(const string& what, HRESULT errorCode) :
    m_ErrorCode(errorCode),
    m_What(what)
{
}

HRESULT Check(HRESULT hr, const char* file, int line)
{
	if (SUCCEEDED(hr))
        return hr;

	WCHAR message[MAX_ERROR_TEXT_LEN];
	DWORD length = AMGetErrorText(hr, message, MAX_ERROR_TEXT_LEN);
    string utf8 = length > 0 ? WideCharToUTF8NoExcept(message) : string();

    ostringstream s;
    if (!utf8.empty())
        s << utf8;
    else
        s << "HRESULT 0x" << hex << hr << dec;
    s << " in " << file << ":" << line;
	
    throw COMException(s.str(), hr);
}
