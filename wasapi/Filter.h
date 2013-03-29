#pragma once
#include "Pin.h"

namespace DirectShow
{
	class Filter
	{
	public:
		Filter(const IID& clsid);
		Filter(LPCOLESTR clsid);
		Filter(const std::wstring& clsid);
		Filter(IBaseFilter* ptr);
		Pin FindPin(LPCWSTR name);
		Pin FindPin(const std::wstring& name);
		operator IBaseFilter*();
		operator const IBaseFilter*() const;
		//Pin operator[](unsigned int index);
		Pin operator[](LPCWSTR name);
		Pin operator[](const std::wstring& name);
	private:
		CComPtr<IBaseFilter> baseFilter;
	};
}