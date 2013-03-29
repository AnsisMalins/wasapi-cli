#include "stdafx.h"
#include "DeviceCollection.h"
#include "com_exception.h"

using namespace COM;
using namespace WASAPI;

DeviceCollection::DeviceCollection(IMMDeviceCollection* ptr) :
	deviceCollection(ptr)
{
}

UINT DeviceCollection::GetCount() const
{
	UINT result;
	HR(deviceCollection->GetCount(&result));
	return result;
}

Device DeviceCollection::Item(UINT index) const
{
	CComPtr<IMMDevice> ptr;
	HR(deviceCollection->Item(index, &ptr));
	return Device(ptr);
}

DeviceCollection::operator IMMDeviceCollection *()
{
	return deviceCollection;
}

DeviceCollection::operator const IMMDeviceCollection *() const
{
	return deviceCollection;
}

Device DeviceCollection::operator [](UINT index) const
{
	return Item(index);
}