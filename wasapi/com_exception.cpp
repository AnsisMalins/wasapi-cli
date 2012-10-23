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
	ostringstream what;
	what << "hr = " << hr << " in " << what_arg;
	throw com_exception(what.str(), hr);
}