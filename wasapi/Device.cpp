#include "stdafx.h"
#include "Device.h"
#include "com_exception.h"

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
	HR(device->GetId(&id));
	wstring result(id);
	CoTaskMemFree(id);
	return result;
}

DWORD Device::GetState() const
{
	DWORD result;
	HR(device->GetState(&result));
	return result;
}

PropertyStore Device::OpenPropertyStore() const
{
	CComPtr<IPropertyStore> ptr;
	HR(device->OpenPropertyStore(STGM_READ, &ptr));
	return PropertyStore(ptr);
}

/*AudioClient Device::ToAudioClient(DWORD params) const
{
	CComPtr<IAudioClient> ptr;
	HR(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&ptr));
	return AudioClient(ptr, params);
}*/

Filter Device::ToFilter() const
{
	CComPtr<IBaseFilter> ptr;
	HR(device->Activate(__uuidof(IBaseFilter), CLSCTX_ALL, NULL, (void**)&ptr));
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