#pragma once

class wexception : public std::exception
{
public:
	wexception(const wchar_t* message);
	wexception(const wchar_t* message, const wchar_t* context);
	void add_context(const wchar_t* context);
	virtual const char* what() const;
	virtual const wchar_t* wwhat() const;
private:
	std::string _what;
	std::wstring _wwhat;
};

#define EX(x) (throw_on_error((x), CONTEXTW))