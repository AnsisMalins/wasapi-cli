#include "stdafx.h"
#include "Device.h"
#include "wexception.h"

using namespace COM;
using namespace DirectShow;
using namespace std;
using namespace WASAPI;

Device::Device(IMMDevice* ptr) :
	device(ptr)
{
}

wstring Device::GetId() const
{
	LPWSTR id;
	EX(device->GetId(&id));
	wstring result(id);
	CoTaskMemFree(id);
	return result;
}

DWORD Device::GetState() const
{
	DWORD result;
	EX(device->GetState(&result));
	return result;
}

Const<PropertyStore> Device::OpenPropertyStore() const
{
	CComPtr<IPropertyStore> ptr;
	EX(device->OpenPropertyStore(STGM_READ, &ptr));
	return Const<PropertyStore>(PropertyStore(ptr));
}

/*AudioClient Device::ToAudioClient(DWORD params) const
{
	CComPtr<IAudioClient> ptr;
	EX(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&ptr));
	return AudioClient(ptr, params);
}*/

Filter Device::ToFilter() const
{
	CComPtr<IBaseFilter> ptr;
	EX(device->Activate(__uuidof(IBaseFilter), CLSCTX_ALL, NULL, (void**)&ptr));
	return Filter(ptr);
}

Device::operator IMMDevice*()
{
	return device;
}

Device::operator const IMMDevice*() const
{
	return device;
}