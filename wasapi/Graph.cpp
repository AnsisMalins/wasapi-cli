#include "stdafx.h"
#include "Graph.h"
#include "com_exception.h"

using namespace DirectShow;
using namespace std;

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveFromRot(DWORD pdwRegister);

Graph::Graph(const IID& clsid, bool addToRot) :
	rotEntry(0)
{
	HR(graphBuilder.CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER));
	if (addToRot) HR(AddToRot(graphBuilder, &rotEntry));
	HR(graphBuilder.QueryInterface(&mediaControl));
}

Graph::~Graph()
{
	if (rotEntry == 0) return;
	graphBuilder.p->AddRef();
	if (graphBuilder.p->Release() <= 1) RemoveFromRot(rotEntry);
}

void Graph::AddFilter(Filter& filter, LPCWSTR name)
{
	HR(graphBuilder->AddFilter(filter, name));
}

Filter Graph::AddSourceFilter(LPCWSTR fileName, LPCWSTR filterName)
{
	CComPtr<IBaseFilter> ptr;
	HR(graphBuilder->AddSourceFilter(fileName, filterName, &ptr));
	return Filter(ptr);
}

void Graph::Connect(Pin& pinOut, Pin& pinIn)
{
	HR(graphBuilder->Connect(pinOut, pinIn));
}

void Graph::ConnectDirect(Pin& pinOut, Pin& pinIn)
{
	HR(graphBuilder->ConnectDirect(pinOut, pinIn, NULL));
}

void Graph::Disconnect(Pin& pin)
{
	HR(graphBuilder->Disconnect(pin));
}

Filter Graph::FindFilter(LPCWSTR name) const
{
	CComPtr<IBaseFilter> ptr;
	HR(graphBuilder->FindFilterByName(name, &ptr));
	return Filter(ptr);
}

void Graph::Pause()
{
	HR(mediaControl->Pause());
}

void Graph::Reconnect(Pin& pin)
{
	HR(graphBuilder->Reconnect(pin));
}

void Graph::RemoveFilter(Filter& filter)
{
	HR(graphBuilder->RemoveFilter(filter));
}

void Graph::Render(Pin& pin)
{
	HR(graphBuilder->Render(pin));
}

void Graph::RenderFile(LPCWSTR fileName)
{
	HR(graphBuilder->RenderFile(fileName, NULL));
}

void Graph::Run()
{
	HR(mediaControl->Run());
}

void Graph::SetLogFile(DWORD_PTR fileHandle)
{
	HR(graphBuilder->SetLogFile(fileHandle));
}

void Graph::Stop()
{
	HR(mediaControl->Stop());
}

wstring Graph::ToString() const
{
	wostringstream result;
	CComPtr<IEnumFilters> enumFilters;
	graphBuilder->EnumFilters(&enumFilters);
	for (CComPtr<IBaseFilter> filter; enumFilters->Next(1, &filter, NULL) == S_OK; filter.Release())
	{
		FILTER_INFO filterInfo;
		filter->QueryFilterInfo(&filterInfo);
		result << filterInfo.achName << endl;
		CComPtr<IEnumPins> enumPins;
		filter->EnumPins(&enumPins);
		for (CComPtr<IPin> pin; enumPins->Next(1, &pin, NULL) == S_OK; pin.Release())
		{
			PIN_INFO pinInfo;
			pin->QueryPinInfo(&pinInfo);
			result << (pinInfo.dir == PINDIR_INPUT ? "<--" : "-->");
			result << " " << pinInfo.achName;
			CComPtr<IPin> connectedTo;
			pin->ConnectedTo(&connectedTo);
			if (pinInfo.dir == PINDIR_OUTPUT && connectedTo != NULL)
			{
				connectedTo->QueryPinInfo(&pinInfo);
				pinInfo.pFilter->QueryFilterInfo(&filterInfo);
				result << " -- " << pinInfo.achName;
				result << " -- " << filterInfo.achName;
			}
			result << endl;
		}
	}
	return result.str();
}

Filter Graph::operator [](LPCWSTR name) const
{
	return FindFilter(name);
}

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker = NULL;
    IRunningObjectTable *pROT = NULL;

    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
 
   StringCchPrintfW(
        wsz, STRING_LENGTH, 
        L"Graph %08x pid %08x", 
        (DWORD_PTR)pUnkGraph, 
        GetCurrentProcessId()
        );
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    
    return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}