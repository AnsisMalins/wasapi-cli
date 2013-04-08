#include "stdafx.h"
#include "win_exception.h"

win_exception::win_exception(
	const wchar_t* message, DWORD err) :
	wexception(message),
	_err(err)
{
}

DWORD win_exception::err() const
{
	return _err;
}

BOOL throw_on_error(BOOL success, const wchar_t* context)
{
	if (success) return success;
	DWORD err = GetLastError();
	LPWSTR errMsg = NULL;
	DWORD errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, err, LANG_NEUTRAL, (LPWSTR)&errMsg, 0, NULL);
	if (errLen == 0)
	{
		va_list args = NULL;
		va_start(args, err);
		errLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING,
			L"err %1!u!%0", 0, 0, (LPWSTR)&errMsg, 0, &args);
		va_end(args);
	}
	win_exception ex(errMsg, err);
	LocalFree(errMsg);
	ex.add_context(context);
	throw ex;
}