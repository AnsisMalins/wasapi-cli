#include "stdafx.h"
#include "args_vector.h"
#include "com_exception.h"
#include "CoInitializer.h"
#include "DeviceEnumerator.h"

using namespace std;

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveFromRot(DWORD pdwRegister);

int wmain(int argc, wchar_t** argv)
{
	DWORD reg = 0;
	try
	{
		args_vector args(argc, argv);
		CoInitializer coinit(COINIT_MULTITHREADED);
		DeviceEnumerator enumerator;
		Device device = enumerator.GetDefaultDevice(eCapture, eMultimedia);
		CComPtr<IBaseFilter> filter = device.GetBaseFilter();

		FILTER_INFO filterInfo;
		HR(filter->QueryFilterInfo(&filterInfo));
		wcout << filterInfo.achName << endl;

		{
			CComPtr<IEnumPins> enumPins;
			HR(filter->EnumPins(&enumPins));
			for (CComPtr<IPin> pin; enumPins->Next(1, &pin, NULL) == S_OK; pin.Release())
			{
				PIN_INFO pinInfo;
				HR(pin->QueryPinInfo(&pinInfo));
				wcout << (pinInfo.dir == PINDIR_INPUT ? "in: " : "out: ") << pinInfo.achName << endl;
			}
		}

		CComPtr<IGraphBuilder> graphBuilder;
		HR(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID*)&graphBuilder));
		HR(AddToRot(graphBuilder, &reg));

		HR(graphBuilder->AddFilter(filter, L"Wasapi"));

		CComPtr<IPin> pin2;
		HR(filter->FindPin(L"Capture", NULL));
		HR(graphBuilder->Render(pin2));
		
		CComPtr<IMediaControl> control;
		HR(graphBuilder.QueryInterface(&control));
		HR(control->Run());
	}
	catch (const exception& e)
	{
		wcout << e.what() << endl;
	}
#ifdef _DEBUG
	WCHAR dummy;
	wcin.getline(&dummy, 1);
#endif
	RemoveFromRot(reg);
	return 0;
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
        L"FilterGraph %08x pid %08x", 
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