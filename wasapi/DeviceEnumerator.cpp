#include "stdafx.h"
#include "DeviceEnumerator.h"
#include "com_exception.h"

using namespace std;

DeviceEnumerator::DeviceEnumerator()
{
	HR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&base));
}

Device DeviceEnumerator::GetDefaultDevice(EDataFlow dataFlow, ERole role) const
{
	CComPtr<IMMDevice> device;
	HR(base->GetDefaultAudioEndpoint(dataFlow, role, &device));
	return Device(device);
}

Device DeviceEnumerator::GetDevice(const wstring& id) const
{
	CComPtr<IMMDevice> device;
	HR(base->GetDevice(id.c_str(), &device));
	return Device(device);
}