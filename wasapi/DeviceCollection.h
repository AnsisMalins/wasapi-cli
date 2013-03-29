#pragma once
#include "Device.h"

namespace WASAPI
{
	class DeviceCollection
	{
	public:
		DeviceCollection(IMMDeviceCollection* ptr);
		UINT GetCount() const;
		Device Item(UINT index) const;
		operator IMMDeviceCollection*();
		operator const IMMDeviceCollection*() const;
		Device operator [](UINT index) const;
	private:
		CComPtr<IMMDeviceCollection> deviceCollection;
	};
}