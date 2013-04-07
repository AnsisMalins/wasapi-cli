#include "stdafx.h"
#include "AudioClient.h"
#include "com_exception.h"

using namespace COM;
using namespace std;
using namespace WASAPI;

AudioClient::AudioClient(IAudioClient* ptr) :
	audioClient(ptr)
{
}

void AudioClient::GetBuffer(BYTE** buffer, UINT32* frameCount)
{
	if (renderClient != NULL)
	{
		EX(renderClient->GetBuffer(bufferSize, buffer));
		*frameCount = bufferSize;
	}
	else
	{
		DWORD flags;
		EX(captureClient->GetBuffer(buffer, frameCount, &flags, NULL, NULL));
		bufferSize = *frameCount;
		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			memset(buffer, 0, *frameCount * frameSize);
	}
}

UINT32 AudioClient::GetBufferSize() const
{
	return bufferSize;
}

UINT32 AudioClient::GetCurrentPadding() const
{
	UINT32 result;
	EX(audioClient->GetCurrentPadding(&result));
	return result * frameSize;
}

REFERENCE_TIME AudioClient::GetDevicePeriod() const
{
	REFERENCE_TIME result;
	EX(audioClient->GetDevicePeriod(&result, NULL));
	return result;
}

WAVEFORMATEX AudioClient::GetMixFormat() const
{
	WAVEFORMATEX* ptr;
	EX(audioClient->GetMixFormat(&ptr));
	WAVEFORMATEX result = *ptr;
	CoTaskMemFree(ptr);
	result.cbSize = 0;
	return result;
}

REFERENCE_TIME AudioClient::GetStreamLatency() const
{
	REFERENCE_TIME result;
	EX(audioClient->GetStreamLatency(&result));
	return result;
}

void AudioClient::Initialize(DWORD params)
{
	WAVEFORMATEX* format;
	EX(audioClient->GetMixFormat(&format));
	HRESULT hr = audioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED, params, 0, 0, format, NULL);
	frameSize = format->nBlockAlign;
	CoTaskMemFree(format);
	EX(hr);
	EX(audioClient->GetBufferSize(&bufferSize));
	hr = audioClient->GetService(
		__uuidof(IAudioRenderClient), (void**)&renderClient);
	if (hr == E_NOINTERFACE)
		EX(audioClient->GetService(
			__uuidof(IAudioCaptureClient), (void**)&captureClient));
	else
		EX(hr);
}

bool AudioClient::IsFormatSupported(const WAVEFORMATEX& format)
{
	WAVEFORMATEX* closestMatch;
	bool result = audioClient->IsFormatSupported(
		AUDCLNT_SHAREMODE_SHARED, &format, &closestMatch) == S_OK;
	CoTaskMemFree(closestMatch);
	return result;
}

void AudioClient::ReleaseBuffer()
{
	if (renderClient != NULL)
		EX(renderClient->ReleaseBuffer(bufferSize, 0));
	else
		EX(captureClient->ReleaseBuffer(bufferSize));
}

void AudioClient::Reset()
{
	EX(audioClient->Reset());
}

void AudioClient::SetEventHandle(HANDLE eventHandle)
{
	EX(audioClient->SetEventHandle(eventHandle));
}

void AudioClient::Start()
{
	EX(audioClient->Start());
}

void AudioClient::Stop()
{
	EX(audioClient->Stop());
}