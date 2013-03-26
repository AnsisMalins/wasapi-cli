#pragma once
#include "Device.h"

namespace WASAPI
{
	class DeviceEnumerator
	{
	public:
		DeviceEnumerator();
		Device GetDefaultDevice(EDataFlow dataFlow, ERole role) const;
		Device GetDevice(LPCWSTR id) const;
		operator IMMDeviceEnumerator*() const;
	private:
		CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	};
}