#pragma once
#include "Pin.h"

namespace DirectShow
{
	class Filter
	{
	public:
		Filter(const IID& clsid);
		Filter(LPCOLESTR clsid);
		Filter(IBaseFilter* ptr);
		Pin FindPin(LPCWSTR name) const;
		operator IBaseFilter*() const;
		Pin operator [](LPCWSTR name) const;
	private:
		CComPtr<IBaseFilter> baseFilter;
	};
}