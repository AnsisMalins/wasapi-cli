#include "stdafx.h"
#include "AudioClient.h"
#include "com_exception.h"

using namespace COM;
using namespace WASAPI;

AudioClient::AudioClient(IAudioClient* ptr, DWORD params) :
	audioClient(ptr)
{
	WAVEFORMATEX* format;
	HR(audioClient->GetMixFormat(&format));
	HRESULT hr = audioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED, params, 0, 0, format, NULL);
	CoTaskMemFree(format);
	HR(hr);
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

bool AudioClient::IsFormatSupported(const WAVEFORMATEX& format)
{
	WAVEFORMATEX* closestMatch;
	bool result = audioClient->IsFormatSupported(
		AUDCLNT_SHAREMODE_SHARED, &format, &closestMatch) == S_OK;
	CoTaskMemFree(closestMatch);
	return result;
}