#pragma once

namespace DirectShow
{
	class Pin
	{
	public:
		Pin(IPin* ptr);
		operator IPin*() const;
	private:
		CComPtr<IPin> pin;
	};
}