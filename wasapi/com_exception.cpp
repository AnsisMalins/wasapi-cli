#include "stdafx.h"
#include "com_exception.h"

using namespace std;

com_exception::com_exception(string& what_arg, HRESULT hr) :
	_hr(hr),
	_what(what_arg)
{
}

HRESULT com_exception::hr() const
{
	return _hr;
}

const char* com_exception::what() const
{
	return _what.c_str();
}

HRESULT throw_on_error(HRESULT hr, const char* what_arg)
{
	if (SUCCEEDED(hr)) return hr;
	wostringstream what;
	WCHAR errMsg[MAX_ERROR_TEXT_LEN];
	DWORD errLen = AMGetErrorText(hr, errMsg, MAX_ERROR_TEXT_LEN);
	if (errLen > 0) what << errMsg;
	else what << "hr " << hr;
	what << " in " << what_arg;
	wstring whatStr = what.str();
	throw com_exception(string(whatStr.begin(), whatStr.end()), hr);
}