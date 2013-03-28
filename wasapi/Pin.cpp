#include "stdafx.h"
#include "Pin.h"
#include "com_exception.h"

using namespace DirectShow;
using namespace std;

Pin::Pin(IPin* ptr) :
	pin(ptr)
{
}

void Pin::Connect(Pin& receivePin)
{
	HR(pin->Connect(receivePin, NULL));
}

Pin Pin::ConnectedTo()
{
	CComPtr<IPin> ptr;
	HR(pin->ConnectedTo(&ptr));
	return Pin(ptr);
}

const Pin Pin::ConnectedTo() const
{
	CComPtr<IPin> ptr;
	HR(pin->ConnectedTo(&ptr));
	return Pin(ptr);
}

void Pin::Disconnect()
{
	HR(pin->Disconnect());
}

PIN_DIRECTION Pin::QueryDirection() const
{
	PIN_DIRECTION result;
	HR(pin->QueryDirection(&result));
	return result;
}

wstring Pin::QueryId() const
{
	LPWSTR id;
	HR(pin->QueryId(&id));
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