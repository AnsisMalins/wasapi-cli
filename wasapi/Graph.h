#pragma once
#include "com_iterator.h"
#include "Filter.h"

namespace DirectShow
{
	class Graph
	{
	public:
		typedef com_iterator<Filter, IBaseFilter, IEnumFilters> iterator;
		Graph(const IID& clsid, bool addToRot);
		~Graph();
		void Abort();
		void AddFilter(Filter& filter, LPCWSTR name);
		void AddFilter(Filter& filter, const std::wstring& name);
		Filter AddSourceFilter(LPCWSTR fileName, LPCWSTR filterName);
		Filter AddSourceFilter(LPCWSTR fileName, const std::wstring& filterName);
		Filter AddSourceFilter(const std::wstring& fileName, LPCWSTR filterName);
		Filter AddSourceFilter(const std::wstring& fileName, const std::wstring& filterName);
		iterator begin();
		void Connect(Pin& pinOut, Pin& pinIn);
		void ConnectDirect(Pin& pinOut, Pin& pinIn);
		void Disconnect(Pin& pin);
		iterator end();
		Filter FindFilter(LPCWSTR name);
		Filter FindFilter(const std::wstring& name);
		long GetEvent();
		HANDLE GetEventHandle() const;
		void Pause();
		void Reconnect(Pin& pin);
		void RemoveFilter(Filter& filter);
		void Render(Pin& pin);
		void RenderFile(LPCWSTR fileName);
		void RenderFile(const std::wstring& fileName);
		void Run();
		void SetLogFile(DWORD_PTR fileHandle);
		void Stop();
		std::wstring ToString() const;
		long WaitForCompletion();
		operator IGraphBuilder*();
		operator const IGraphBuilder*() const;
		operator IMediaControl*();
		operator const IMediaControl*() const;
		Filter operator [](const std::wstring& name);
		Filter operator [](LPCWSTR name);
	private:
		CComPtr<IGraphBuilder> graphBuilder;
		CComPtr<IMediaControl> mediaControl;
		CComPtr<IMediaEvent> mediaEvent;
		DWORD rotEntry;
		static HANDLE sigint;
	};
}