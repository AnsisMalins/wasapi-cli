#pragma once
#include "Filter.h"

namespace DirectShow
{
	class Graph
	{
	public:
		Graph(const IID& clsid, bool addToRot);
		~Graph();
		void AddFilter(Filter& filter, LPCWSTR name);
		Filter AddSourceFilter(LPCWSTR fileName, LPCWSTR filterName);
		void Connect(Pin& pinOut, Pin& pinIn);
		void ConnectDirect(Pin& pinOut, Pin& pinIn);
		void Disconnect(Pin& pin);
		Filter FindFilter(LPCWSTR name) const;
		void Pause();
		void Reconnect(Pin& pin);
		void RemoveFilter(Filter& filter);
		void Render(Pin& pin);
		void RenderFile(LPCWSTR fileName);
		void Run();
		void SetLogFile(DWORD_PTR fileHandle);
		void Stop();
		std::wstring ToString() const;
		Filter operator [](LPCWSTR name) const;
	private:
		CComPtr<IGraphBuilder> graphBuilder;
		CComPtr<IMediaControl> mediaControl;
		DWORD rotEntry;
	};
}