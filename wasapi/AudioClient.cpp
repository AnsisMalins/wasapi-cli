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
		HR(renderClient->GetBuffer(bufferSize, buffer));
		*frameCount = bufferSize;
	}
	else
	{
		DWORD flags;
		HR(captureClient->GetBuffer(buffer, frameCount, &flags, NULL, NULL));
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
	HR(audioClient->GetCurrentPadding(&result));
	return result * frameSize;
}

REFERENCE_TIME AudioClient::GetDevicePeriod() const
{
	REFERENCE_TIME result;
	HR(audioClient->GetDevicePeriod(&result, NULL));
	return result;
}

WAVEFORMATEX AudioClient::GetMixFormat() const
{
	WAVEFORMATEX* ptr;
	HR(audioClient->GetMixFormat(&ptr));
	WAVEFORMATEX result = *ptr;
	CoTaskMemFree(ptr);
	result.cbSize = 0;
	return result;
}

REFERENCE_TIME AudioClient::GetStreamLatency() const
{
	REFERENCE_TIME result;
	HR(audioClient->GetStreamLatency(&result));
	return result;
}

void AudioClient::Initialize(DWORD params)
{
	WAVEFORMATEX* format;
	HR(audioClient->GetMixFormat(&format));
	HRESULT hr = audioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED, params, 0, 0, format, NULL);
	frameSize = format->nBlockAlign;
	CoTaskMemFree(format);
	HR(hr);
	HR(audioClient->GetBufferSize(&bufferSize));
	hr = audioClient->GetService(
		__uuidof(IAudioRenderClient), (void**)&renderClient);
	if (hr == E_NOINTERFACE)
		HR(audioClient->GetService(
			__uuidof(IAudioCaptureClient), (void**)&captureClient));
	else
		HR(hr);
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
		HR(renderClient->ReleaseBuffer(bufferSize, 0));
	else
		HR(captureClient->ReleaseBuffer(bufferSize));
}

void AudioClient::Reset()
{
	HR(audioClient->Reset());
}

void AudioClient::SetEventHandle(HANDLE eventHandle)
{
	HR(audioClient->SetEventHandle(eventHandle));
}

void AudioClient::Start()
{
	HR(audioClient->Start());
}

void AudioClient::Stop()
{
	HR(audioClient->Stop());
}