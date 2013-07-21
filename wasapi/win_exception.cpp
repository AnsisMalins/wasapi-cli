#include "stdafx.h"
#include "win_exception.h"

win_exception::win_exception(
	const wchar_t* message, DWORD err_code) :
	wexception(message),
	_err_code(err_code)
{
}

DWORD win_exception::err_code() const
{
	return _err_code;
}

BOOL throw_on_error(BOOL success, const wchar_t* context)
{
	if (success) return success;
	DWORD err = GetLastError();
	LPWSTR errMsg = NULL;
	DWORD errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, err, LANG_NEUTRAL, (LPWSTR)&errMsg, 0, NULL);
	if (errLen == 0) errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING,
		L"winerr %1!u!%0", 0, 0, (LPWSTR)&errMsg, 0, (va_list*)&err);
	win_exception ex(errMsg, err);
	LocalFree(errMsg);
	ex.add_context(context);
	throw ex;
}