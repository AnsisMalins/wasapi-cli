#include "stdafx.h"
#include "com_exception.h"
#include "CoInitializer.h"
#include "DeviceEnumerator.h"
#include "Graph.h"
#include "StdoutRenderer.h"

using namespace COM;
using namespace DirectShow;
using namespace std;
using namespace WASAPI;

void get_next_arg(const vector<wstring>& args, vector<wstring>::const_iterator& arg, const char* message)
{
	if (++arg == args.end()) throw invalid_argument(message);
}

HANDLE hSigint;
void on_sigint(int sigint)
{
	SetEvent(hSigint);
}

bool try_get_next_arg(const vector<wstring>& args, vector<wstring>::const_iterator& arg)
{
	return ++arg != args.end();
}

void Main(const vector<wstring>& args)
{
	CoInitializer coinit(COINIT_MULTITHREADED);

	Graph graph(CLSID_FilterGraph, true);

	hSigint = CreateEvent(NULL, TRUE, FALSE, NULL);
	signal(SIGINT, on_sigint);

	vector<wstring>::const_iterator arg = args.begin();
	if (!try_get_next_arg(args, arg) || *arg == L"/?" || *arg == L"--help")
	{
		cout << "Usage:" << endl
			<< "-a  --add-filter <filter type> <filter name>" << endl
			<< "-c  --connect <filter1> <pin1> <filter2> <pin2>" << endl
			<< "-l  --list-devices <mask>" << endl
			<< "-f  --add-file <file name> <filter name>" << endl
			<< "-p  --print-graph" << endl
			<< "-r  --render-pin <filter name> <pin name>" << endl;
		return;
	}
	while (true)
	{
		if (*arg == L"-a" || *arg == L"--add-filter")
		{
			get_next_arg(args, arg, "--add-filter filter type missing");
			Filter filter((IBaseFilter*)NULL);
			if (*arg == L"stdout")
			{
				HRESULT hr = S_OK;
				filter = Filter(new StdoutRenderer(&hr));
				HR(hr);
			}
			else if (*arg == L"wasapi")
			{
				get_next_arg(args, arg, "--add-filter wasapi id missing");
				DeviceEnumerator wasapi;
				Device device(NULL);
				if (*arg == L"capture-communications")
					device = wasapi.GetDefaultDevice(eCapture, eCommunications);
				else if (*arg == L"capture-multimedia")
					device = wasapi.GetDefaultDevice(eCapture, eMultimedia);
				else if (*arg == L"render-communications")
					device = wasapi.GetDefaultDevice(eRender, eCommunications);
				else if (*arg == L"render-multimedia")
					device = wasapi.GetDefaultDevice(eRender, eMultimedia);
				else
					device = wasapi.GetDevice(*arg);
				/*if (*(arg + 1) == L"loopback")
				{
					++arg;
					filter = device.ToFilter();
				}
				else*/ filter = device.ToFilter();
			}
			else
			{
				filter = Filter(*arg);
			}
			get_next_arg(args, arg, "--add-filter name missing");
			graph.AddFilter(filter, *arg);
		}
		else if (*arg == L"-c" || *arg == L"--connect")
		{
			get_next_arg(args, arg, "--connect first filter name missing");
			wstring filter1 = *arg;
			get_next_arg(args, arg, "--connect output pin name missing");
			wstring pin1 = *arg;
			get_next_arg(args, arg, "--connect second filter name missing");
			wstring filter2 = *arg;
			get_next_arg(args, arg, "--connect input pin name missing");
			graph.Connect(graph[filter1][pin1], graph[filter2][*arg]);
		}
		else if (*arg == L"-l" || *arg == L"--list-devices")
		{
			get_next_arg(args, arg, "--list-devices mask missing");
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
			DeviceCollection devices = DeviceEnumerator().EnumDevices(dataFlow, stateMask);
			for (UINT i = 0; i < devices.GetCount(); i++)
			{
				Device device = devices[i];
				PROPVARIANT name = device.OpenPropertyStore()[PKEY_Device_FriendlyName];
				wcout << device.GetId() << endl << name.pwszVal << endl << endl;
				PropVariantClear(&name);
			}
			return;
		}
		else if (*arg == L"-f" || *arg == L"--add-file")
		{
			get_next_arg(args, arg, "--add-file file name missing");
			wstring fileName = *arg;
			get_next_arg(args, arg, "--add-file filter name missing");
			graph.AddSourceFilter(fileName, *arg);
		}
		else if (*arg == L"-p" || *arg == L"--print-graph")
		{
			wcout << graph.ToString();
			return;
		}
		else if (*arg == L"-r" || *arg == L"--render-pin")
		{
			get_next_arg(args, arg, "--render-pin filter name missing");
			wstring filterName = *arg;
			get_next_arg(args, arg, "--render-pin pin name missing");
			graph.Render(graph[filterName][*arg]);
		}
		else
		{
			wostringstream what;
			what << "invalid command: " << *arg;
			wstring what_str = what.str();
			throw invalid_argument(string(what_str.begin(), what_str.end()));
		}

		if (!try_get_next_arg(args, arg)) break;
	}

	graph.Run();
	HANDLE handles[] = { hSigint, graph.GetEventHandle() };
	while (true)
	{
		if (WaitForMultipleObjects(sizeof(handles) / sizeof(HANDLE), handles, FALSE, INFINITE) == 0
			|| graph.GetEvent() == EC_COMPLETE)
			break;
	}
	graph.Stop();
}

int wmain(int argc, wchar_t** argv)
{
	int result = 0;
	try
	{
		vector<wstring> args;
		args.reserve(argc);
#ifdef _DEBUG
		args.push_back(wstring(L""));

		args.push_back(wstring(L"--add-filter"));
		args.push_back(wstring(L"wasapi"));
		args.push_back(wstring(L"capture-multimedia"));
		args.push_back(wstring(L"capture"));

		args.push_back(wstring(L"--add-filter"));
		args.push_back(wstring(L"wasapi"));
		args.push_back(wstring(L"render-multimedia"));
		args.push_back(wstring(L"render"));

		args.push_back(wstring(L"--connect"));
		args.push_back(wstring(L"capture"));
		args.push_back(wstring(L"Capture"));
		args.push_back(wstring(L"render"));
		args.push_back(wstring(L"Audio Input pin (rendered)"));
		
		//args.push_back(wstring(L"--print-graph"));
#else
		for (int i = 0; i < argc; i++) args.push_back(wstring(argv[i]));
#endif
		Main(args);
	}
	catch (const com_exception& ex)
	{
		wcout << ex.what() << endl;
		result = ex.hr();
	}
	catch (const exception& ex)
	{
		wcout << ex.what() << endl;
		result = 1;
	}
#ifdef _DEBUG
	WCHAR dummy;
	wcin.getline(&dummy, 1);
#endif
	return result;
}