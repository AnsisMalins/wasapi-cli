#include "stdafx.h"
#include "Pin.h"

using namespace DirectShow;

Pin::Pin(IPin* ptr) :
	pin(ptr)
{
}

Pin::operator IPin *() const
{
	return pin;
}