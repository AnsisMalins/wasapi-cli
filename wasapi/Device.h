#pragma once
#include "Filter.h"

namespace WASAPI
{
	class Device
	{
	public:
		Device(IMMDevice* ptr);
		DirectShow::Filter ToFilter() const;
		operator IMMDevice*() const;
	private:
		CComPtr<IMMDevice> device;
	};
}