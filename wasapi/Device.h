#pragma once

class Device
{
public:
	Device(const CComPtr<IMMDevice>& device);
	CComPtr<IBaseFilter> GetBaseFilter() const;
private:
	CComPtr<IMMDevice> base;
};