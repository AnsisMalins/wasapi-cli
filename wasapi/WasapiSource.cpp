#include "stdafx.h"
#include "WasapiSource.h"
#include "DeviceEnumerator.h"

using namespace DirectShow;

WasapiSource::WasapiSource(LPCWSTR id, HRESULT* phr) :
	CSource(NAME("WasapiSource"), NULL, GUID_NULL, phr),
	m_Pin(this, id, phr)
{
}

WasapiSource::Pin::Pin(CSource* pms, LPCWSTR id, HRESULT* phr) :
	CSourceStream(NAME("Capture"), phr, pms, L"1"),
	m_hBufferReady(NULL),
	m_Initialized(FALSE),
	m_rtPrevious(0)
{
	if (id == NULL && phr != NULL) *phr = E_POINTER;

	CComPtr<IMMDeviceEnumerator> pDeviceEnumerator;
	HRESULT hr = pDeviceEnumerator.CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hr))
	{
		*phr = hr;
		return;
	}

	CComPtr<IMMDevice> pDevice;
	hr = pDeviceEnumerator->GetDevice(id, &pDevice);
	if (FAILED(hr))
	{
		*phr = hr;
		return;
	}

	CComPtr<IMMEndpoint> pEndpoint;
	hr = pDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pEndpoint);
	if (FAILED(hr))
	{
		*phr = hr;
		return;
	}

	hr = pEndpoint->GetDataFlow(&m_eDataFlow);
	if (FAILED(hr))
	{
		*phr = hr;
		return;
	}

	hr = pDevice->Activate(
		__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
	if (FAILED(hr))
	{
		*phr = hr;
		return;
	}

	if (m_eDataFlow == eCapture) return;

	hr = pDevice->Activate(
		__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pEventClient);
	if (FAILED(hr))
	{
		*phr = hr;
		return;
	}
}

HRESULT WasapiSource::Pin::CheckMediaType(const CMediaType* pMediaType)
{
	if (pMediaType == NULL) return E_POINTER;
	if (*pMediaType->FormatType() != FORMAT_WaveFormatEx) return E_FAIL;
	if (pMediaType->Format() == NULL) return E_FAIL;

	WAVEFORMATEX* pClosestMatch = NULL;
	HRESULT hr = m_pAudioClient->IsFormatSupported(
		AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)pMediaType->Format(),
		&pClosestMatch);
	if (pClosestMatch != NULL) CoTaskMemFree(pClosestMatch);

	return hr == S_OK ? S_OK : E_FAIL;
}

HRESULT WasapiSource::Pin::DecideBufferSize(
	IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest)
{
	if (ppropInputRequest == NULL) return E_POINTER;

	HRESULT hr = Initialize();
	if (FAILED(hr)) return hr;

	UINT32 nBufferFrames;
	hr = m_pAudioClient->GetBufferSize(&nBufferFrames);
	if (FAILED(hr)) return hr;

	long cbBuffer = nBufferFrames * m_cbFrame;
	ppropInputRequest->cbBuffer = max(ppropInputRequest->cbBuffer, cbBuffer);
	ppropInputRequest->cBuffers = max(ppropInputRequest->cBuffers, 2);

	ALLOCATOR_PROPERTIES actual;
	hr = pAlloc->SetProperties(ppropInputRequest, &actual);
	if (FAILED(hr)) return hr;

	if (actual.cbBuffer < cbBuffer) return E_FAIL;

	return S_OK;
}

HRESULT WasapiSource::Pin::FillBuffer(IMediaSample* pSample)
{
	WaitForSingleObject(m_hBufferReady, INFINITE);

	BYTE* pbSample;
	HRESULT hr = pSample->GetPointer(&pbSample);
	if (FAILED(hr)) return hr;

	BYTE* pbClient;
	UINT32 nClient;
	DWORD dwFlags;
	UINT64 u64MediaTime, u64Time;
	hr = m_pCaptureClient->GetBuffer(
		&pbClient, &nClient, &dwFlags, &u64MediaTime, &u64Time);
	if (FAILED(hr)) return hr;
	
	UINT32 cbClient = nClient * m_cbFrame;
	if((long)cbClient > pSample->GetSize())
	{
		m_pCaptureClient->ReleaseBuffer(0);
		return E_FAIL;
	}

	if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT) memset(pbSample, 0, cbClient);
	else memcpy(pbSample, pbClient, cbClient);

	hr = m_pCaptureClient->ReleaseBuffer(nClient);
	if (FAILED(hr)) return hr;

	hr = pSample->SetActualDataLength(cbClient);
	if (FAILED(hr)) return hr;

	pSample->SetDiscontinuity(
		dwFlags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY);
	if (FAILED(hr)) return hr;

	// The following section causes a deadlock.
	/*LONGLONG llMediaTimeStart = u64MediaTime;
	LONGLONG llMediaTimeEnd = llMediaTimeStart + nClient;
	hr = pSample->SetMediaTime(&llMediaTimeStart, &llMediaTimeEnd);
	if (FAILED(hr)) return hr;

	REFERENCE_TIME rtEnd = u64Time;
	hr = pSample->SetTime(&m_rtPrevious, &rtEnd);
	if (FAILED(hr)) return hr;

	m_rtPrevious = rtEnd;*/

	return S_OK;
}

HRESULT WasapiSource::Pin::GetMediaType(CMediaType* pMediaType)
{
	if (pMediaType == NULL) return E_POINTER;
	
	pMediaType->InitMediaType();
	pMediaType->SetType(&MEDIATYPE_Audio);
	pMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
	pMediaType->SetFormatType(&FORMAT_WaveFormatEx);
	
	WAVEFORMATEX* pFormat;
	HRESULT hr = m_pAudioClient->GetMixFormat(&pFormat);
	if (FAILED(hr)) return hr;

	pMediaType->pbFormat = (BYTE*)pFormat;
	pMediaType->cbFormat = sizeof(WAVEFORMATEX) + pFormat->cbSize;
	pMediaType->SetSampleSize(pFormat->nBlockAlign);
	m_cbFrame = pFormat->nBlockAlign;

	return S_OK;
}

HRESULT WasapiSource::Pin::Initialize()
{
	if (m_Initialized) return S_FALSE;

	WAVEFORMATEX* pDeviceFormat;
	HRESULT hr = m_pAudioClient->GetMixFormat(&pDeviceFormat);
	if (FAILED(hr)) return hr;

	hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
		m_eDataFlow == eCapture
			? AUDCLNT_STREAMFLAGS_EVENTCALLBACK
			: AUDCLNT_STREAMFLAGS_LOOPBACK,
		0, 0, pDeviceFormat, NULL);
	CoTaskMemFree(pDeviceFormat);
	if (FAILED(hr)) return hr;

	hr = m_pAudioClient->GetService(
		__uuidof(IAudioCaptureClient), (void**)&m_pCaptureClient);
	if (FAILED(hr)) return hr;

	m_hBufferReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hBufferReady == NULL) return HRESULT_FROM_WIN32(GetLastError());

	if (m_eDataFlow == eRender)
	{
		hr = m_pEventClient->GetMixFormat(&pDeviceFormat);
		if (FAILED(hr)) return hr;

		hr = m_pEventClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			0, 0, pDeviceFormat, NULL);
		CoTaskMemFree(pDeviceFormat);
		if (FAILED(hr)) return hr;

		hr = m_pEventClient->SetEventHandle(m_hBufferReady);
		if (FAILED(hr)) return hr;
	}
	else
	{
		hr = m_pAudioClient->SetEventHandle(m_hBufferReady);
		if (FAILED(hr)) return  hr;
	}

	m_Initialized = TRUE;

	return S_OK;
}

HRESULT WasapiSource::Pin::OnThreadDestroy()
{
	HRESULT hr = m_pAudioClient->Stop();

	if (m_eDataFlow == eCapture) return hr;

	HRESULT hr2 = m_pEventClient->Stop();
	if (FAILED(hr2)) return hr;

	return hr;
}

HRESULT WasapiSource::Pin::OnThreadStartPlay()
{
	HRESULT hr = m_pAudioClient->Start();
	if (FAILED(hr)) return hr;

	if (m_eDataFlow == eCapture) return S_OK;

	hr = m_pEventClient->Start();
	if (FAILED(hr))
	{
		m_pAudioClient->Stop();
		return hr;
	}

	return S_OK;
}