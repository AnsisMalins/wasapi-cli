#include "stdafx.h"
#include "DeviceEnumerator.h"
#include "wexception.h"

using namespace COM;
using namespace std;
using namespace WASAPI;

DeviceEnumerator::DeviceEnumerator()
{
	EX(deviceEnumerator.CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER));
}

DeviceCollection DeviceEnumerator::EnumDevices(EDataFlow dataFlow, DWORD stateMask) const
{
	CComPtr<IMMDeviceCollection> ptr;
	EX(deviceEnumerator->EnumAudioEndpoints(dataFlow, stateMask, &ptr));
	return DeviceCollection(ptr);
}

Device DeviceEnumerator::GetDefaultDevice(EDataFlow dataFlow, ERole role) const
{
	CComPtr<IMMDevice> ptr;
	EX(deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, role, &ptr));
	return Device(ptr);
}

Device DeviceEnumerator::GetDevice(LPCWSTR id) const
{
	CComPtr<IMMDevice> ptr;
	EX(deviceEnumerator->GetDevice(id, &ptr));
	return Device(ptr);
}

Device DeviceEnumerator::GetDevice(const wstring& id) const
{
	return GetDevice(id.c_str());
}

DeviceEnumerator::operator IMMDeviceEnumerator *()
{
	return deviceEnumerator;
}

DeviceEnumerator::operator const IMMDeviceEnumerator *() const
{
	return deviceEnumerator;
}