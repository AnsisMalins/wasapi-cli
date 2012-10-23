#pragma once
#include "Device.h"

class DeviceEnumerator
{
public:
	DeviceEnumerator();
	Device GetDefaultDevice(EDataFlow dataFlow, ERole role) const;
	Device GetDevice(const std::wstring& id) const;
private:
	CComPtr<IMMDeviceEnumerator> base;
};