#include "StdAfx.h"
#include "sock_exception.h"

sock_exception::sock_exception(const wchar_t* message, int error) :
	wexception(message),
	_error(error)
{
}

int throw_on_socket_error(int socket_error, const wchar_t* context)
{
	if (socket_error != SOCKET_ERROR) return socket_error;
	int err = WSAGetLastError();
	LPWSTR errMsg = NULL;
	DWORD errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, err, LANG_NEUTRAL, (LPWSTR)&errMsg, 0, NULL);
	if (errLen == 0) errLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING,
		L"sockerr %1!u!%0", 0, 0, (LPWSTR)&errMsg, 0, (va_list*)&err);
	sock_exception ex(errMsg, err);
	LocalFree(errMsg);
	ex.add_context(context);
	throw ex;
}