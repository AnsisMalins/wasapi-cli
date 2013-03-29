#include "stdafx.h"
#include "PropertyStore.h"
#include "com_exception.h"

using namespace COM;

PropertyStore::PropertyStore(IPropertyStore* ptr) :
	propertyStore(ptr)
{
}

void PropertyStore::Commit()
{
	HR(propertyStore->Commit());
}

PROPERTYKEY PropertyStore::GetAt(DWORD index) const
{
	PROPERTYKEY result;
	HR(propertyStore->GetAt(index, &result));
	return result;
}

DWORD PropertyStore::GetCount() const
{
	DWORD result;
	HR(propertyStore->GetCount(&result));
	return result;
}

PROPVARIANT PropertyStore::GetValue(REFPROPERTYKEY key) const
{
	PROPVARIANT result;
	PropVariantInit(&result);
	HR(propertyStore->GetValue(key, &result));
	return result;
}

/*void PropertyStore::SetValue(REFPROPERTYKEY key, REFPROPVARIANT value)
{
	HR(propertyStore->SetValue(key, value));
}*/

PropertyStore::operator IPropertyStore *()
{
	return propertyStore;
}

PropertyStore::operator const IPropertyStore *() const
{
	return propertyStore;
}

PROPVARIANT PropertyStore::operator [](REFPROPERTYKEY key) const
{
	return GetValue(key);
}