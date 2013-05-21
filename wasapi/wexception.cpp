#include "stdafx.h"
#include "wexception.h"

using namespace std;

wexception::wexception(const wchar_t* message) :
	_wwhat(message), _what(_wwhat.begin(), _wwhat.end())
{
}

wexception::wexception(const wchar_t* message, const wchar_t* context) :
	_wwhat(message)
{
	add_context(context);
}

void wexception::add_context(const wchar_t* context)
{
	wostringstream str;
	str << _wwhat << endl << context;
	_wwhat = str.str();
	_what = string(_wwhat.begin(), _wwhat.end());
}

const char* wexception::what() const
{
	return _what.c_str();
}

const wchar_t* wexception::wwhat() const
{
	return _wwhat.c_str();
}