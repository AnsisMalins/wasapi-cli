#include "stdafx.h"
#include "Filter.h"
#include "com_exception.h"

using namespace DirectShow;
using namespace std;

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

Pin Filter::FindPin(LPCWSTR name)
{
	CComPtr<IPin> ptr;
	HR(baseFilter->FindPin(name, &ptr));
	return Pin(ptr);
}

const Pin Filter::FindPin(LPCWSTR name) const
{
	CComPtr<IPin> ptr;
	HR(baseFilter->FindPin(name, &ptr));
	return Pin(ptr);
}

Pin Filter::FindPin(const wstring& name)
{
	return FindPin(name.c_str());
}

const Pin Filter::FindPin(const wstring& name) const
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

Pin Filter::operator [](LPCWSTR name)
{
	return FindPin(name);
}

const Pin Filter::operator [](LPCWSTR name) const
{
	return FindPin(name);
}

Pin Filter::operator [](const wstring& name)
{
	return FindPin(name.c_str());
}

const Pin Filter::operator [](const wstring& name) const
{
	return FindPin(name.c_str());
}