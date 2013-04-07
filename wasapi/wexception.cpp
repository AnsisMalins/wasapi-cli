#include "stdafx.h"
#include "wexception.h"

using namespace std;

wstring& wexception::trace()
{
	return _trace;
}

const char* wexception::what() const
{
	//_trace.append(L" ");
	
	return NULL;
}

const wchar_t* wexception::wwhat() const
{
	return NULL;
}


win_exception::win_exception(DWORD err) :
	_err(err)
{
	LPWSTR errMsg;
	DWORD errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, err, LANG_NEUTRAL, (LPWSTR)&errMsg, 0, NULL);

}


BOOL throw_on_error(BOOL success, const char* context)
{
	if (success) return success;
	wostringstream what;
	DWORD err = GetLastError();
	LPWSTR errMsg;
	DWORD errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, err, LANG_NEUTRAL, (LPWSTR)&errMsg, 0, NULL);
	if (errLen > 0) what << errMsg;
	else what << "err " << err;
	what << " in " << context;
	wstring what_str = what.str();
	win_exception ex(err);
	throw ex;
}