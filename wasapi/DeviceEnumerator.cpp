#include "stdafx.h"
#include "DeviceEnumerator.h"
#include "com_exception.h"

using namespace std;
using namespace WASAPI;

DeviceEnumerator::DeviceEnumerator()
{
	HR(deviceEnumerator.CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER));
}

DeviceCollection DeviceEnumerator::EnumDevices(EDataFlow dataFlow, DWORD stateMask) const
{
	CComPtr<IMMDeviceCollection> ptr;
	HR(deviceEnumerator->EnumAudioEndpoints(dataFlow, stateMask, &ptr));
	return DeviceCollection(ptr);
}

Device DeviceEnumerator::GetDefaultDevice(EDataFlow dataFlow, ERole role) const
{
	CComPtr<IMMDevice> ptr;
	HR(deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, role, &ptr));
	return Device(ptr);
}

Device DeviceEnumerator::GetDevice(LPCWSTR id) const
{
	CComPtr<IMMDevice> ptr;
	HR(deviceEnumerator->GetDevice(id, &ptr));
	return Device(ptr);
}

DeviceEnumerator::operator IMMDeviceEnumerator *()
{
	return deviceEnumerator;
}

DeviceEnumerator::operator const IMMDeviceEnumerator *() const
{
	return deviceEnumerator;
}