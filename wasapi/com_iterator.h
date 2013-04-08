#pragma once

namespace COM
{
	template<typename T, typename TUnk, typename TEnum>
	class com_iterator : public std::iterator<std::input_iterator_tag, TUnk>
	{
	public:
		com_iterator() :
			begin(false)
		{
		}
		com_iterator(const com_iterator& it)
		{
			begin = it.begin;
			it.pEnum->Clone(&pEnum);
			unk = it.unk;
		}
		TEnum** init()
		{
			begin = true;
			return &pEnum;
		}
		com_iterator& operator ++()
		{
			if (pEnum == NULL) return *this;
			CComPtr<TUnk> pUnk;
			pEnum->Next(1, &pUnk, NULL);
			unk = T(pUnk);
			return *this;
		}
		com_iterator operator ++(int)
		{
			com_iterator result(*this);
			operator ++();
			return result;
		}
		bool operator ==(const com_iterator& it) const
		{
			return begin == it.begin && unk == it.unk;
		}
		bool operator !=(const com_iterator& it) const
		{
			return begin == it.begin && unk != it.unk;
		}
		T& operator *()
		{
			return unk;
		}
		T* operator ->()
		{
			return &unk;
		}
	private:
		bool begin;
		CComPtr<TEnum> pEnum;
		T unk;
	};
}