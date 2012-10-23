#include "stdafx.h"
#include "Device.h"
#include "com_exception.h"

Device::Device(const CComPtr<IMMDevice>& device) :
	base(device)
{
}

CComPtr<IBaseFilter> Device::GetBaseFilter() const
{
	CComPtr<IBaseFilter> baseFilter;
	HR(base->Activate(IID_IBaseFilter, CLSCTX_ALL, NULL, (void**)&baseFilter));
	return baseFilter;
}