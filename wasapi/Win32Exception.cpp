#include "stdafx.h"
#include "Win32Exception.h"
#include "Encoding.h"

using namespace std;

Win32Exception::Win32Exception(const std::string& what, DWORD errorCode) :
    m_ErrorCode(errorCode),
    m_What(what)
{
}

BOOL Check(BOOL success, const char* file, int line)
{
	if (success)
        return success;

	DWORD error = GetLastError();

    LPWSTR message = NULL;
    DWORD length = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, error, LANG_NEUTRAL, (LPWSTR)&message, 0, NULL);
    string utf8 = length > 0 ? WideCharToUTF8NoExcept(message) : string();
    if (message != NULL)
        LocalFree(message);

    ostringstream s;
    if (!utf8.empty())
        s << utf8;
    else
        s << "Error 0x" << hex << error << dec;
    s << " in " << file << ":" << line;

    throw Win32Exception(s.str(), error);
}
