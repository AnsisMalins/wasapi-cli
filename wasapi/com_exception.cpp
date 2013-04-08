#include "stdafx.h"
#include "com_exception.h"

using namespace COM;

com_exception::com_exception(
	const wchar_t* message, HRESULT hr) :
	wexception(message),
	_hr(hr)
{
}

HRESULT com_exception::hr() const
{
	return _hr;
}

HRESULT throw_on_error(HRESULT hr, const wchar_t* context)
{
	if (SUCCEEDED(hr)) return hr;
	WCHAR errMsg[MAX_ERROR_TEXT_LEN];
	DWORD errLen = AMGetErrorText(hr, errMsg, MAX_ERROR_TEXT_LEN);
	if (errLen == 0) wnsprintf(errMsg, MAX_ERROR_TEXT_LEN, L"hr %u", hr);
	com_exception ex(errMsg, hr);
	ex.add_context(context);
	throw ex;
}