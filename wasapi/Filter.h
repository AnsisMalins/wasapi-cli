#pragma once
#include "com_iterator.h"
#include "Pin.h"

namespace DirectShow
{
	class Filter
	{
	public:
		typedef com_iterator<Pin, IPin, IEnumPins> iterator;
		Filter();
		Filter(const IID& clsid);
		Filter(LPCOLESTR clsid);
		Filter(const std::wstring& clsid);
		Filter(IBaseFilter* ptr);
		iterator begin();
		iterator end();
		Pin FindPin(LPCWSTR name);
		Pin FindPin(const std::wstring& name);
		Pin In(unsigned int index);
		Pin Out(unsigned int index);
		operator IBaseFilter*();
		operator const IBaseFilter*() const;
		Pin operator[](const std::wstring& name);
		Pin operator[](LPCWSTR name);
	private:
		CComPtr<IBaseFilter> baseFilter;
	};
}