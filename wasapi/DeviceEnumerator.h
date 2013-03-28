#pragma once
#include "Device.h"
#include "DeviceCollection.h"

namespace WASAPI
{
	class DeviceEnumerator
	{
	public:
		DeviceEnumerator();
		DeviceCollection EnumDevices(EDataFlow dataFlow, DWORD stateMask) const;
		Device GetDefaultDevice(EDataFlow dataFlow, ERole role) const;
		Device GetDevice(LPCWSTR id) const;
		operator IMMDeviceEnumerator*();
		operator const IMMDeviceEnumerator*() const;
	private:
		CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	};
}