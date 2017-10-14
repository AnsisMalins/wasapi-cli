#include "stdafx.h"
#include "Filter.h"
#include "COMException.h"

#define EX(errorCode) Check((errorCode), __FILE__, __LINE__)

using namespace DirectShow;
using namespace std;

Filter::Filter()
{
}

Filter::Filter(const IID& clsid)
{
	EX(baseFilter.CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER));
}

Filter::Filter(LPCOLESTR clsid)
{
	CLSID clsid2;
	EX(CLSIDFromString(clsid, &clsid2));
	EX(baseFilter.CoCreateInstance(clsid2, NULL, CLSCTX_INPROC_SERVER));
}

Filter::Filter(const wstring& clsid)
{
	CLSID clsid2;
	EX(CLSIDFromString(clsid.c_str(), &clsid2));
	EX(baseFilter.CoCreateInstance(clsid2, NULL, CLSCTX_INPROC_SERVER));
}

Filter::Filter(IBaseFilter* ptr) :
	baseFilter(ptr)
{
}

Filter::iterator Filter::begin()
{
	iterator result;
	EX(baseFilter->EnumPins(result.init()));
	return ++result;
}

Filter::iterator Filter::end()
{
	return iterator();
}

Pin Filter::FindPin(LPCWSTR name)
{
	CComPtr<IPin> ptr;
	EX(baseFilter->FindPin(name, &ptr));
	return Pin(ptr);
}

Pin Filter::In(unsigned int index)
{
	++index;
	for (iterator i = begin(); i != end(); ++i)
		if (i->QueryDirection() == PINDIR_INPUT && --index == 0)
            return *i;

    ostringstream s;
    s << "Filter::In out of range in " << __FILE__ << ":" << __LINE__;
    throw out_of_range(s.str());
}

Pin Filter::Out(unsigned int index)
{
	++index;
	for (iterator i = begin(); i != end(); ++i)
		if (i->QueryDirection() == PINDIR_OUTPUT && --index == 0)
            return *i;

    ostringstream s;
    s << "Filter::Out out of range in " << __FILE__ << ":" << __LINE__;
    throw out_of_range(s.str());
}

Pin Filter::FindPin(const wstring& name)
{
	return FindPin(name.c_str());
}

Filter::operator IBaseFilter*()
{
	return baseFilter;
}

Filter::operator const IBaseFilter*() const
{
	return baseFilter;
}

Pin Filter::operator [](const wstring& name)
{
	return FindPin(name.c_str());
}

Pin Filter::operator [](LPCWSTR name)
{
	return FindPin(name);
}

/*Pin Filter::operator [](unsigned int index)
{
	CComPtr<IEnumPins> enumPins;
	EX(baseFilter->EnumPins(&enumPins));
	CComPtr<IPin> ptr;
	index++;
	while (index > 0)
	{
		ptr.Release();
		index--;
		HRESULT hr = enumPins->Next(1, &ptr, NULL);
		if (EX(hr) != S_OK)
			throw out_of_range("EnumPins out of range.\n" CONTEXT);
	}
	return Pin(ptr);
}*/