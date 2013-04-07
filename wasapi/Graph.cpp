#include "stdafx.h"
#include "Graph.h"
#include "com_exception.h"

using namespace COM;
using namespace DirectShow;
using namespace std;

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveFromRot(DWORD pdwRegister);

Graph::Graph(const IID& clsid, bool addToRot) :
	rotEntry(0)
{
	EX(graphBuilder.CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER));
	if (addToRot) EX(AddToRot(graphBuilder, &rotEntry));
	EX(graphBuilder.QueryInterface(&mediaControl));
	EX(graphBuilder.QueryInterface(&mediaEvent));
}

Graph::~Graph()
{
	if (rotEntry == 0) return;
	graphBuilder.p->AddRef();
	if (graphBuilder.p->Release() <= 1) RemoveFromRot(rotEntry);
}

void Graph::Abort()
{
	EX(graphBuilder->Abort());
}

void Graph::AddFilter(Filter& filter, LPCWSTR name)
{
	EX(graphBuilder->AddFilter(filter, name));
}

void Graph::AddFilter(Filter& filter, const wstring& name)
{
	AddFilter(filter, name.c_str());
}

Filter Graph::AddSourceFilter(LPCWSTR fileName, LPCWSTR filterName)
{
	CComPtr<IBaseFilter> ptr;
	EX(graphBuilder->AddSourceFilter(fileName, filterName, &ptr));
	return Filter(ptr);
}

Filter Graph::AddSourceFilter(LPCWSTR fileName, const wstring& filterName)
{
	return AddSourceFilter(fileName, filterName.c_str());
}

Filter Graph::AddSourceFilter(const wstring& fileName, LPCWSTR filterName)
{
	return AddSourceFilter(fileName.c_str(), filterName);
}

Filter Graph::AddSourceFilter(const wstring& fileName, const wstring& filterName)
{
	return AddSourceFilter(fileName.c_str(), filterName.c_str());
}

void Graph::Connect(Pin& pinOut, Pin& pinIn)
{
	EX(graphBuilder->Connect(pinOut, pinIn));
}

void Graph::ConnectDirect(Pin& pinOut, Pin& pinIn)
{
	EX(graphBuilder->ConnectDirect(pinOut, pinIn, NULL));
}

void Graph::Disconnect(Pin& pin)
{
	EX(graphBuilder->Disconnect(pin));
}

long Graph::GetEvent()
{
	long result;
	LONG_PTR param1, param2;
	EX(mediaEvent->GetEvent(&result, &param1, &param2, INFINITE));
	EX(mediaEvent->FreeEventParams(result, param1, param2));
	return result;
}

HANDLE Graph::GetEventHandle() const
{
	HANDLE result;
	EX(mediaEvent->GetEventHandle((OAEVENT*)&result));
	return result;
}

Filter Graph::FindFilter(LPCWSTR name)
{
	CComPtr<IBaseFilter> ptr;
	EX(graphBuilder->FindFilterByName(name, &ptr));
	return Filter(ptr);
}

Filter Graph::FindFilter(const wstring& name)
{
	return FindFilter(name.c_str());
}

void Graph::Pause()
{
	EX(mediaControl->Pause());
}

void Graph::Reconnect(Pin& pin)
{
	EX(graphBuilder->Reconnect(pin));
}

void Graph::RemoveFilter(Filter& filter)
{
	EX(graphBuilder->RemoveFilter(filter));
}

void Graph::Render(Pin& pin)
{
	EX(graphBuilder->Render(pin));
}

void Graph::RenderFile(LPCWSTR fileName)
{
	EX(graphBuilder->RenderFile(fileName, NULL));
}

void Graph::RenderFile(const wstring& fileName)
{
	RenderFile(fileName.c_str());
}

void Graph::Run()
{
	EX(mediaControl->Run());
}

void Graph::SetLogFile(DWORD_PTR fileHandle)
{
	EX(graphBuilder->SetLogFile(fileHandle));
}

void Graph::Stop()
{
	EX(mediaControl->Stop());
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
			result << (pinInfo.dir == PINDIR_INPUT ? ">--" : "-->");
			result << " " << pinInfo.achName;
			CComPtr<IPin> connectedTo;
			pin->ConnectedTo(&connectedTo);
			if (pinInfo.dir == PINDIR_OUTPUT && connectedTo != NULL)
			{
				connectedTo->QueryPinInfo(&pinInfo);
				pinInfo.pFilter->QueryFilterInfo(&filterInfo);
				result << " --> " << pinInfo.achName;
				result << " >-- " << filterInfo.achName;

				AM_MEDIA_TYPE mt;
				pin->ConnectionMediaType(&mt);
				if (mt.subtype == MEDIASUBTYPE_PCM)
				{
					WAVEFORMATEX& fmt = *(WAVEFORMATEX*)mt.pbFormat;
					result << endl << fmt.nChannels << " channels "
						<< fmt.wBitsPerSample << " bits per sample "
						<< fmt.nSamplesPerSec << " samples per second";
				}
				FreeMediaType(mt);
			}
			result << endl;
		}
	}
	return result.str();
}

long Graph::WaitForCompletion()
{
	long result;
	EX(mediaEvent->WaitForCompletion(INFINITE, &result));
	return result;
}

Graph::operator IGraphBuilder *()
{
	return graphBuilder;
}

Graph::operator const IGraphBuilder *() const
{
	return graphBuilder;
}

Graph::operator IMediaControl *()
{
	return mediaControl;
}

Graph::operator const IMediaControl *() const
{
	return mediaControl;
}

Filter Graph::operator [](LPCWSTR name)
{
	return FindFilter(name);
}

Filter Graph::operator [](const wstring& name)
{
	return FindFilter(name);
}

HANDLE  Graph::sigint = NULL;

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