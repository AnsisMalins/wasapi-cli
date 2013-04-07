#pragma once

#define EX(x) (throw_on_error((x), CONTEXT))
BOOL throw_on_error(BOOL success, const char* context);

class wexception : public std::exception
{
public:
	std::wstring& trace();
	virtual const char* what() const;
	virtual const wchar_t* wwhat() const;
private:
	std::wstring _trace;
};

class win_exception : public wexception
{
public:
	win_exception(DWORD err);
	DWORD err() const;
private:
	DWORD _err;
};