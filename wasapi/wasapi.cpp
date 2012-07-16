#include "stdafx.h"

int wmain(int argc, wchar_t** argv)
{
	if (CoInitializeEx(NULL, COINIT_MULTITHREADED) != S_OK) return __LINE__;
	IMMDeviceEnumerator* deviceEnumerator;
	if (CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator) != S_OK) return __LINE__;
	IMMDevice* device = NULL;
	if (argc == 2 && !wcscmp(argv[1], L"/l"))
	{
		IMMDeviceCollection* deviceCollection;
		if (deviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATEMASK_ALL, &deviceCollection) != S_OK) return __LINE__;
		UINT deviceCount;
		if (deviceCollection->GetCount(&deviceCount) != S_OK) return __LINE__;
		for (UINT i = 0; i < deviceCount; i++)
		{
			if (deviceCollection->Item(i, &device) == S_OK)
			{
				LPWSTR deviceId;
				if (device->GetId(&deviceId) == S_OK)
				{
					wprintf(L"%s\n", deviceId);
					CoTaskMemFree(deviceId);
				}
				device->Release();
			}
		}
		return 0;
	}
	EDataFlow dataFlow = eCapture;
	DWORD streamFlags = 0;
	for (int i = 1; i < argc; i++)
	{
		if (!wcscmp(argv[i], L"/d"))
		{
			i++;
			if (deviceEnumerator->GetDevice(argv[i], &device) != S_OK) return __LINE__;
		}
		else if (!wcscmp(argv[i], L"/r"))
		{
			dataFlow = eRender;
		}
		else if (!wcscmp(argv[i], L"/lb"))
		{
			streamFlags |= AUDCLNT_STREAMFLAGS_LOOPBACK;
		}
	}
	if (device == NULL) if (deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, eMultimedia, &device) != S_OK) return __LINE__;
	deviceEnumerator->Release();
	IAudioClient* audioClient;
	if (device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient) != S_OK) return __LINE__;
	WAVEFORMATEX* format;
	if (audioClient->GetMixFormat(&format) != S_OK) return __LINE__;
	format->wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	format->nBlockAlign = format->wBitsPerSample / 8 * format->nChannels;
	format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
	format->cbSize = 0;
	if (argc == 2 && !wcscmp(argv[1], L"/?"))
	{
		printf("Channels: %hu\nSamples per second: %u\nBits per sample: %hu\n", format->nChannels, format->nSamplesPerSec, format->wBitsPerSample);
		return 0;
	}
	if (audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, 1000000, 0, format, NULL) != S_OK) return __LINE__;
	WORD frameSize = format->nBlockAlign;
	CoTaskMemFree(format);
	IAudioCaptureClient* audioCaptureClient;
	if (audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&audioCaptureClient) != S_OK) return __LINE__;
	if (_setmode(_fileno(stdout), _O_BINARY) == -1) return __LINE__;
	if (audioClient->Start() != S_OK) return __LINE__;
	UINT32 bufferSize;
	audioClient->GetBufferSize(&bufferSize);
	BYTE* buffer = new BYTE[bufferSize * frameSize];
	while (true)
	{
		Sleep(10);
		while (true)
		{
			BYTE* data;
			UINT32 frames;
			DWORD flags;
			HRESULT hr = audioCaptureClient->GetBuffer(&data, &frames, &flags, NULL, NULL);
			if (hr == AUDCLNT_S_BUFFER_EMPTY) break;
			if (FAILED(hr)) return __LINE__;
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT) memset(buffer, 0, frames * frameSize);
			else memcpy(buffer, data, frames * frameSize);
			if (audioCaptureClient->ReleaseBuffer(frames) != S_OK) return __LINE__;
			if (fwrite(data, frameSize, frames, stdout) < frames) return __LINE__;
		}
	}
	return 0;
}