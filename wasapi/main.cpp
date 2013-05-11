#include "stdafx.h"
#include "AudioConverter.h"
#include "com_exception.h"
#include "CoInitializer.h"
#include "DeviceEnumerator.h"
#include "Graph.h"
#include "RtpRenderer.h"
#include "RtpSource.h"
#include "StdoutRenderer.h"
#include "WasapiSource.h"

using namespace COM;
using namespace DirectShow;
using namespace std;
using namespace WASAPI;

void WaitForCompletion(Graph& graph);
void Main(const vector<wstring>& args);
wstring next_arg(const vector<wstring>& args,
	vector<wstring>::const_iterator& arg);
void on_sigint(int sig);
WAVEFORMATEX wavefmt(WORD wFormatTag, WORD nChannels,
	DWORD nSamplesPerSec, WORD wBitsPerSample);

HANDLE hSigint;

void Main(const vector<wstring>& args)
{
	hSigint = CreateEvent(NULL, TRUE, FALSE, NULL);
	signal(SIGINT, on_sigint);

	CoInitializer coinit(COINIT_MULTITHREADED);

	Graph graph(CLSID_FilterGraph, true);

	vector<wstring>::const_iterator arg = args.begin();
	++arg;

	if (arg == args.end() || *arg == L"/?" || *arg == L"--help")
	{
		cout << "Usage:" << endl
			<< "wasapi <source> ! <sink>" << endl
			<< "source = filesrc | rtpsrc | wasapisrc" << endl
			<< "sink   = rtpsink | stdsink | wasapisink" << endl;
		return;
	}
	else if (*arg == L"-l")
	{
		next_arg(args, arg);
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

	Filter curFilter;

	while (arg != args.end())
	{
		Filter newFilter;
		wstring filterName;
		if (*arg == L"audioconvert")
		{
			wstring formatStr = next_arg(args, arg);
			if (formatStr == L"!") throw invalid_argument("audioconvert needs format");
			WAVEFORMATEX format;
			if (formatStr == L"s16le") format = wavefmt(WAVE_FORMAT_PCM, 2, 44100, 16);
			else throw invalid_argument("audioconvert: format not supported");
			HRESULT hr = S_OK;
			CComPtr<AudioConverter> audioconvert = new AudioConverter(format, &hr);
			EX(hr);
			newFilter = Filter(audioconvert);
			filterName = L"audioconvert";
		}
		else if (*arg == L"filesink")
		{
			throw invalid_argument("filesink is not implemented yet\n" CONTEXT);
		}
		else if (*arg == L"filesrc")
		{
			wstring fileName = next_arg(args, arg);
			filterName = next_arg(args, arg);
			if (filterName == L"!") filterName = L"filesrc";
			curFilter = graph.AddSourceFilter(fileName, filterName);
		}
		else if (*arg == L"print")
		{
			wcout << graph.ToString();
			return;
		}
		else if (*arg == L"render")
		{
			graph.Render(curFilter.Out(0));
			wcout << graph.ToString();
			break;
		}
		else if (*arg == L"rtpsink")
		{
			wstring lipwstr = next_arg(args, arg);
			if (lipwstr == L"!") throw invalid_argument("rtpsink needs local IP");
			wstring ripwstr = next_arg(args, arg);
			if (ripwstr == L"!") throw invalid_argument("rtpsink needs remote IP");
			wstring rportwstr = next_arg(args, arg);
			if (rportwstr == L"!") throw invalid_argument("rtpsink needs remote port");
			string lipstr(lipwstr.begin(), lipwstr.end());
			IN_ADDR localIP;
			localIP.s_addr = inet_addr(lipstr.c_str());
			string ripstr(ripwstr.begin(), ripwstr.end());
			IN_ADDR remoteIP;
			remoteIP.s_addr = inet_addr(ripstr.c_str());
			USHORT remotePort = _wtoi(rportwstr.c_str());
			SOCKADDR_IN remoteEP = { AF_INET, htons(remotePort), remoteIP };
			HRESULT hr = S_OK;
			CComPtr<RtpRenderer> rtpsink = new RtpRenderer(localIP, remoteEP, &hr);
			EX(hr);
			newFilter = Filter(rtpsink);
			filterName = L"rtpsink";
		}
		else if (*arg == L"rtpsrc")
		{
			wstring lipwstr = next_arg(args, arg);
			if (lipwstr == L"!") throw invalid_argument("need localip");
			wstring portwstr = next_arg(args, arg);
			if (portwstr == L"!") throw invalid_argument("need port");
			string lipstr(lipwstr.begin(), lipwstr.end());
			SOCKADDR_IN localEP;
			localEP.sin_family = AF_INET;
			localEP.sin_addr.s_addr = inet_addr(lipstr.c_str());
			localEP.sin_port = htons(_wtoi(portwstr.c_str()));
			HRESULT hr = S_OK;
			CComPtr<RtpSource> rtpsrc = new RtpSource(localEP, &hr);
			EX(hr);
			newFilter = Filter(rtpsrc);
			filterName = L"rtpsrc";
		}
		else if (*arg == L"stdsink")
		{
			HRESULT hr = S_OK;
			newFilter = Filter(new StdoutRenderer(&hr));
			EX(hr);
			filterName = L"stdsink";
		}
		else if (*arg == L"stdsrc")
		{
			throw invalid_argument("stdsrc is not implemented yet\n" CONTEXT);
		}
		else if (*arg == L"wasapisink")
		{
			wstring id = next_arg(args, arg);
			DeviceEnumerator wasapi;
			Device device;
			if (id == L"eCommunications")
				device = wasapi.GetDefaultDevice(eRender, eCommunications);
			else if (id == L"eConsole" || id == L"!")
				device = wasapi.GetDefaultDevice(eRender, eConsole);
			else if (id == L"eMultimedia")
				device = wasapi.GetDefaultDevice(eRender, eMultimedia);
			else
				device = wasapi.GetDevice(id);
			newFilter = device.ToFilter();
			filterName = L"wasapisink";
		}
		else if (*arg == L"wasapisrc")
		{
			wstring id = next_arg(args, arg);
			bool defaultDevice = true;
			EDataFlow dataFlow;
			if (id == L"eCapture" || id == L"!") dataFlow = eCapture;
			else if (id == L"eRender") dataFlow = eRender;
			else defaultDevice = false;
			if (defaultDevice)
			{
				if (id != L"!") id = next_arg(args, arg);
				ERole role;
				if (id == L"eCommunications") role = eCommunications;
				else if (id == L"eConsole" || id == L"!") role = eConsole;
				else if (id == L"eMultimedia") role = eMultimedia;
				else throw invalid_argument("wasapisrc: Invalid ERole\n" CONTEXT);
				DeviceEnumerator wasapi;
				Device device;
				id = wasapi.GetDefaultDevice(dataFlow, role).GetId();
			}
			HRESULT hr = S_OK;
			newFilter = Filter(new WasapiSource(id.c_str(), &hr));
			EX(hr);
			filterName = L"wasapisrc";
		}
		else
		{
			wostringstream str;
			str << "unknown command: " << *arg << "\n" CONTEXT;
			wstring what = str.str();
			throw invalid_argument(string(what.begin(), what.end()));
		}

		if (newFilter != NULL && arg != args.end() && *arg != L"!")
		{
			wstring tmp = next_arg(args, arg);
			if (tmp != L"!")
			{
				filterName = next_arg(args, arg);
				if (next_arg(args, arg) != L"!")
					throw invalid_argument("\"!\" expected\n" CONTEXT);
			}
		}
		if (newFilter != NULL)
		{
			graph.AddFilter(newFilter, filterName);
			if (curFilter != NULL)
				graph.ConnectDirect(curFilter.Out(0), newFilter.In(0));
		}
		curFilter = newFilter;
		next_arg(args, arg);
	}
	
	graph.Run();
	WaitForCompletion(graph);
	graph.Stop();
}

wstring next_arg(const vector<wstring>& args,
	vector<wstring>::const_iterator& arg)
{
	if (arg != args.end()) ++arg;
	return arg != args.end() ? *arg : L"!";
}

void on_sigint(int sigint)
{
	if (!SetEvent(hSigint))
	{
		//INVALID_HANDLE_VALUE
	}
}

void WaitForCompletion(Graph& graph)
{
	HANDLE h[] = { hSigint, graph.GetEventHandle() };
	DWORD n = sizeof(h) / sizeof(HANDLE);
	while (WaitForMultipleObjects(n, h, FALSE, INFINITE) != 0
		&& graph.GetEvent() != EC_COMPLETE) ;
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

		args.push_back(wstring(L"wasapisrc"));
		args.push_back(wstring(L"eCapture"));

		args.push_back(wstring(L"!"));

		args.push_back(wstring(L"audioconvert"));
		args.push_back(wstring(L"s16le"));

		args.push_back(wstring(L"!"));

		args.push_back(wstring(L"rtpsink"));
		args.push_back(wstring(L"127.0.0.1"));
		args.push_back(wstring(L"127.0.0.1"));
		args.push_back(wstring(L"5004"));

		args.push_back(wstring(L"!"));
		args.push_back(wstring(L"print"));

#else
		for (int i = 0; i < argc; i++) args.push_back(wstring(argv[i]));
#endif
		Main(args);
	}
	catch (const com_exception& ex)
	{
		wcerr << ex.what() << endl;
		result = ex.hr();
	}
	catch (const exception& ex)
	{
		wcerr << ex.what() << endl;
		result = 1;
	}
#ifdef _DEBUG
	WaitForSingleObject(hSigint, INFINITE);
#endif
	return result;
}

WAVEFORMATEX wavefmt(WORD wFormatTag, WORD nChannels,
	DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	WAVEFORMATEX wf;
	wf.wFormatTag = wFormatTag;
	wf.nChannels = nChannels;
	wf.nSamplesPerSec = nSamplesPerSec;
	wf.wBitsPerSample = wBitsPerSample;
	wf.nBlockAlign = wBitsPerSample / 8 * nChannels;
	wf.nAvgBytesPerSec = wf.nBlockAlign * nSamplesPerSec;
	wf.cbSize = 0;
	return wf;
}