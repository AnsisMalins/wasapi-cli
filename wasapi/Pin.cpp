#include "stdafx.h"
#include "Pin.h"
#include "com_exception.h"

using namespace COM;
using namespace DirectShow;
using namespace std;

Pin::Pin()
{
}

Pin::Pin(IPin* ptr) :
	pin(ptr)
{
}

void Pin::Connect(Pin& receivePin)
{
	EX(pin->Connect(receivePin, NULL));
}

Pin Pin::ConnectedTo()
{
	CComPtr<IPin> ptr;
	EX(pin->ConnectedTo(&ptr));
	return Pin(ptr);
}

AM_MEDIA_TYPE Pin::ConnectionMediaType() const
{
	AM_MEDIA_TYPE result;
	EX(pin->ConnectionMediaType(&result));
	return result;
}

void Pin::Disconnect()
{
	EX(pin->Disconnect());
}

PIN_DIRECTION Pin::QueryDirection() const
{
	PIN_DIRECTION result;
	EX(pin->QueryDirection(&result));
	return result;
}

wstring Pin::QueryId() const
{
	LPWSTR id;
	EX(pin->QueryId(&id));
	wstring result(id);
	CoTaskMemFree(id);
	return result;
}

Pin::operator IPin*()
{
	return pin;
}

Pin::operator const IPin*() const
{
	return pin;
}