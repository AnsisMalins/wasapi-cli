#pragma once
#include "wexception.h"

namespace COM
{
	class com_exception : public wexception
	{
	public:
		com_exception(const wchar_t*, HRESULT hr);
		HRESULT hr() const;
	private:
		HRESULT _hr;
	};
}

HRESULT throw_on_error(HRESULT hr, const wchar_t* context);