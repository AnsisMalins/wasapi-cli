#include "stdafx.h"
#include "PropertyStore.h"
#include "wexception.h"

using namespace COM;

PropertyStore::PropertyStore()
{
}

PropertyStore::PropertyStore(IPropertyStore* ptr) :
	propertyStore(ptr)
{
}

void PropertyStore::Commit()
{
	//if (propertyStore == NULL) throw POINTER_
	EX(propertyStore->Commit());
}

PROPERTYKEY PropertyStore::GetAt(DWORD index) const
{
	PROPERTYKEY result;
	EX(propertyStore->GetAt(index, &result));
	return result;
}

DWORD PropertyStore::GetCount() const
{
	DWORD result;
	EX(propertyStore->GetCount(&result));
	return result;
}

PROPVARIANT PropertyStore::GetValue(REFPROPERTYKEY key) const
{
	PROPVARIANT result;
	PropVariantInit(&result);
	EX(propertyStore->GetValue(key, &result));
	return result;
}

void PropertyStore::SetValue(REFPROPERTYKEY key, REFPROPVARIANT value)
{
	EX(propertyStore->SetValue(key, value));
}

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