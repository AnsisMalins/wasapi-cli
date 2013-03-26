#include "stdafx.h"
#include "Device.h"
#include "com_exception.h"

using namespace DirectShow;
using namespace WASAPI;

Device::Device(IMMDevice* ptr) :
	device(ptr)
{
}

Filter Device::ToFilter() const
{
	CComPtr<IBaseFilter> ptr;
	HR(device->Activate(IID_IBaseFilter, CLSCTX_ALL, NULL, (void**)&ptr));
	return Filter(ptr);
}

Device::operator IMMDevice*() const
{
	return device;
}