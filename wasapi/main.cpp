#include "stdafx.h"
#include "com_exception.h"
#include "CoInitializer.h"
#include "DeviceEnumerator.h"
#include "Graph.h"
#include "StdoutRenderer.h"

using namespace DirectShow;
using namespace std;
using namespace WASAPI;

Filter FilterFromArg(const wstring& arg)
{
	if (arg == L"stdout")
	{
		HRESULT hr;
		Filter result(new StdoutRenderer(&hr));
		HR(hr);
		return result;
	}
	else
	{
		return Filter(arg.c_str());
	}
}

void Main(const vector<wstring>& args)
{
	CoInitializer coinit(COINIT_MULTITHREADED);

	DeviceEnumerator wasapi;
	Graph graph(CLSID_FilterGraph, true);

	vector<wstring>::const_iterator arg = args.begin();
	while (arg != args.end())
	{
		if (++arg == args.end()) throw exception("Missing argument.");
		if (*arg == L"-a" || *arg == L"--add-filter")
		{
			if (++arg == args.end()) throw exception("Missing argument.");
			Filter filter = FilterFromArg(*arg);
			if (++arg == args.end()) throw exception("Missing argument.");
			graph.AddFilter(filter, arg->c_str());
		}
		else if (*arg == L"-c" || *arg == L"--connect")
		{
			if (++arg == args.end()) throw exception("Missing argument.");
			wstring filter1 = *arg;
			if (++arg == args.end()) throw exception("Missing argument.");
			wstring pin1 = *arg;
			if (++arg == args.end()) throw exception("Missing argument.");
			wstring filter2 = *arg;
			if (++arg == args.end()) throw exception("Missing argument.");
			wstring pin2 = *arg;
			//graph.Connect(graph[filter1][pin1], graph[filter2][pin2]);
		}
		else if (*arg == L"-d" || *arg == L"--add-device")
		{
		}
		else if (*arg == L"-l" || *arg == L"--list-devices")
		{
			if (++arg == args.end()) throw exception("Missing argument.");
			EDataFlow dataFlow = eAll;
			DWORD stateMask = 0;
			for (wstring::const_iterator i = arg->begin(); i != arg->end(); ++i)
			{
				switch (*i)
				{
				case 'r': dataFlow = eRender; break;
				case 'c': dataFlow = eCapture; break;
				case 'a': stateMask |= DEVICE_STATE_ACTIVE;
				case 'd': stateMask |= DEVICE_STATE_DISABLED;
				case 'n': stateMask |= DEVICE_STATE_NOTPRESENT;
				case 'u': stateMask |= DEVICE_STATE_UNPLUGGED;
				}
			}
			if (stateMask == 0) stateMask = DEVICE_STATEMASK_ALL;
			DeviceCollection devices = wasapi.EnumDevices(dataFlow, stateMask);
			for (UINT i = 0; i < devices.GetCount(); i++)
			{
				Device device = devices[i];
				PROPVARIANT name = device.OpenPropertyStore()[PKEY_Device_FriendlyName];
				wcout << device.GetId() << endl << name.pwszVal << endl << endl;
				PropVariantClear(&name);
			}
			break;
		}
		else if (*arg == L"-n" || *arg == L"--no-run")
		{
			wcout << graph.ToString();
			WCHAR dummy;
			wcin.getline(&dummy, 1);
			return;
		}
		else if (*arg == L"-f" || *arg == L"--add-file")
		{
			if (++arg == args.end()) throw exception("Missing argument.");
			wstring fileName = *arg;
			if (++arg == args.end()) throw exception("Missing argument.");
			graph.AddSourceFilter(fileName.c_str(), arg->c_str());
		}
		else throw exception("Unknown argument.");
	}

	graph.Run();
	WCHAR dummy;
	wcin.getline(&dummy, 1);
}

int wmain(int argc, wchar_t** argv)
{
	try
	{
		vector<wstring> args(argc);
#ifdef _DEBUG
		args.push_back(wstring(L"-l"));
		args.push_back(wstring(L"r"));
#else
		for (int i = 0; i < argc; i++) args.push_back(wstring(argv[i]));
#endif
		Main(args);
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