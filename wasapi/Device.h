#pragma once
#include "Filter.h"
#include "AudioClient.h"
#include "PropertyStore.h"

namespace WASAPI
{
	class Device
	{
	public:
		Device(IMMDevice* ptr);
		std::wstring GetId() const;
		DWORD GetState() const;
		COM::PropertyStore OpenPropertyStore() const;
		//AudioClient ToAudioClient(DWORD params) const;
		DirectShow::Filter ToFilter() const;
		operator IMMDevice*();
		operator const IMMDevice*() const;
	private:
		CComPtr<IMMDevice> device;
	};
}