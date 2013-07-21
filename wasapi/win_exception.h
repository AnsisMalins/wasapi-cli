#pragma once
#include "wexception.h"

class win_exception : public wexception
{
public:
	win_exception(const wchar_t* message, DWORD err);
	DWORD err_code() const;
private:
	DWORD _err_code;
};

BOOL throw_on_error(BOOL success, const wchar_t* context);