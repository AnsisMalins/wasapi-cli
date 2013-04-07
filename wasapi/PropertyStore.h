#pragma once

namespace COM
{
	class PropertyStore
	{
	public:
		PropertyStore();
		PropertyStore(IPropertyStore* ptr);
		void Commit();
		PROPERTYKEY GetAt(DWORD index) const;
		DWORD GetCount() const;
		PROPVARIANT GetValue(REFPROPERTYKEY key) const;
		void SetValue(REFPROPERTYKEY key, REFPROPVARIANT value);
		operator IPropertyStore*();
		operator const IPropertyStore*() const;
		PROPVARIANT operator [](REFPROPERTYKEY key) const;
	private:
		CComPtr<IPropertyStore> propertyStore;
	};
}