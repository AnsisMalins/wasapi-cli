#pragma once

namespace COM
{
	class com_exception : public std::exception
	{
	public:
		com_exception(const char* what_arg, HRESULT hr);
		com_exception(const std::string& what_arg, HRESULT hr);
		HRESULT hr() const throw();
		virtual const char* what() const;
	private:
		std::string _what;
		HRESULT _hr;
	};
}

HRESULT throw_on_error(HRESULT hr, const char* context);

#define HR(x) (throw_on_error((x), __FILE__ ":" STR(__LINE__)))