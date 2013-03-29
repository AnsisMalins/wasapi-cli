#include "stdafx.h"
#include "WasapiSource.h"
#include "DeviceEnumerator.h"

using namespace DirectShow;
using namespace std;
using namespace WASAPI;

WasapiSource::WasapiSource(AudioClient& audioClient, HRESULT* phr) :
	CSource(NAME("WasapiSource"), NULL, GUID_NULL, phr),
	pin(this, audioClient, phr)
{
}

WasapiSource::Pin::Pin(CSource* pms, AudioClient& audioClient, HRESULT* phr) :
	CSourceStream(NAME("Capture"), phr, pms, L"Capture"),
	audioClient(audioClient)
{
}

HRESULT WasapiSource::Pin::CheckMediaType(const CMediaType* mediaType)
{
	if (*mediaType->FormatType() != FORMAT_WaveFormatEx) return E_FAIL;
	return audioClient.IsFormatSupported(*(WAVEFORMATEX*)mediaType->Format())
		? S_OK : E_FAIL;
}

HRESULT WasapiSource::Pin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest)
{
	return E_NOTIMPL;
}

HRESULT WasapiSource::Pin::FillBuffer(IMediaSample *pSample)
{
	return E_NOTIMPL;
}

HRESULT WasapiSource::Pin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if (iPosition < 0) return E_INVALIDARG;
	if (iPosition > 0) return VFW_S_NO_MORE_ITEMS;
	if (pMediaType == NULL) return E_POINTER;
	pMediaType->InitMediaType();
	pMediaType->SetType(&MEDIATYPE_Audio);
	pMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
	pMediaType->SetFormatType(&FORMAT_WaveFormatEx);
	pMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
	WAVEFORMATEX& format = *(WAVEFORMATEX*)pMediaType->Format();
	format = audioClient.GetMixFormat();
	pMediaType->SetSampleSize(format.nBlockAlign);
	return S_OK;
}