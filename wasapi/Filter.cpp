#include "stdafx.h"
#include "Filter.h"
#include "com_exception.h"

using namespace DirectShow;

Filter::Filter(const IID& clsid)
{
	HR(baseFilter.CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER));
}

Filter::Filter(LPCOLESTR clsid)
{
	CLSID clsid2;
	HR(CLSIDFromString(clsid, &clsid2));
	HR(baseFilter.CoCreateInstance(clsid2, NULL, CLSCTX_INPROC_SERVER));
}

Filter::Filter(IBaseFilter* ptr) :
	baseFilter(ptr)
{
}

Pin Filter::FindPin(LPCWSTR name) const
{
	CComPtr<IPin> ptr;
	HR(baseFilter->FindPin(name, &ptr));
	return Pin(ptr);
}

Filter::operator IBaseFilter*() const
{
	return baseFilter;
}

Pin Filter::operator [](LPCWSTR name) const
{
	return FindPin(name);
}