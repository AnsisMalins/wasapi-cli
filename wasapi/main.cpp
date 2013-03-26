#include "stdafx.h"
#include "args_vector.h"
#include "com_exception.h"
#include "CoInitializer.h"
#include "DeviceEnumerator.h"
#include "Graph.h"

using namespace DirectShow;
using namespace std;
using namespace WASAPI;

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void DumpGraph(IFilterGraph* graph);
void Main(const args_vector& args);
void RemoveFromRot(DWORD pdwRegister);

int wmain(int argc, wchar_t** argv)
{
	try
	{
		Main(args_vector(argc, argv));
	}
	catch (const exception& ex)
	{
		wcout << ex.what() << endl;
#ifdef _DEBUG
	WCHAR dummy;
	wcin.getline(&dummy, 1);
#endif
		return 1;
	}
	return 0;
}

void Main(const args_vector& args)
{
	CoInitializer coinit(COINIT_MULTITHREADED);

	DeviceEnumerator wasapi;
	Graph graph(CLSID_FilterGraph, true);
	graph.AddFilter(wasapi.GetDefaultDevice(eCapture, eMultimedia).ToFilter(), L"in");
	graph.AddFilter(wasapi.GetDefaultDevice(eRender, eMultimedia).ToFilter(), L"out");
	graph.Connect(graph[L"in"][L"Capture"], graph[L"out"][L"Audio Input pin (rendered)"]);
	graph.Run();

	wcout << graph.ToString();

	WCHAR dummy;
	wcin.getline(&dummy, 1);
}