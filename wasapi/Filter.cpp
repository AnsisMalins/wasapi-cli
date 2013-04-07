#include "stdafx.h"
#include "Filter.h"
#include "com_exception.h"

using namespace COM;
using namespace DirectShow;
using namespace std;

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

Pin Filter::FindPin(LPCWSTR name)
{
	CComPtr<IPin> ptr;
	EX(baseFilter->FindPin(name, &ptr));
	return Pin(ptr);
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

/*Pin Filter::operator [](unsigned int index)
{
	CComPtr<IEnumPins> enumPins;
	EX(baseFilter->EnumPins(&enumPins));
	CComPtr<IPin> ptr;
	index++;
	while (index > 0)
	{
		index--;
		if (enumPins->Next(1, &ptr, NULL) != S_OK)
			throw out_of_range(

	}
	for (; index >= 0; index--, ) enumPins->Next(1, 
}*/

Pin Filter::operator [](LPCWSTR name)
{
	return FindPin(name);
}

Pin Filter::operator [](const wstring& name)
{
	return FindPin(name.c_str());
}