#pragma once

namespace DirectShow
{
	class Pin
	{
	public:
		Pin(IPin* ptr);
		void Connect(Pin& receivePin);
		Pin ConnectedTo();
		const Pin ConnectedTo() const;
		void Disconnect();
		PIN_DIRECTION QueryDirection() const;
		std::wstring QueryId() const;
		operator IPin*();
		operator const IPin*() const;
	private:
		CComPtr<IPin> pin;
	};
}