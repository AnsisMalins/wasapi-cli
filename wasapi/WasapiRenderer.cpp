#include "stdafx.h"
#include "WasapiRenderer.h"
#include "waveformatex.h"

using namespace DirectShow;

WasapiRenderer::WasapiRenderer(HRESULT* phr) :
	CBaseRenderer(GUID_NULL, NAME("WasapiRenderer"), NULL, phr),
	m_pFormat(NULL)
{
}

WasapiRenderer::~WasapiRenderer()
{
	if (m_pFormat != NULL) CoTaskMemFree(m_pFormat);
}

HRESULT WasapiRenderer::Activate(LPCWSTR pwstrId)
{
	CComPtr<IMMDeviceEnumerator> pDeviceEnumerator;
	HRESULT hr = pDeviceEnumerator.CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;
	
	CComPtr<IMMDevice> pDevice;
	hr = pDeviceEnumerator->GetDevice(pwstrId, &pDevice);
	if (FAILED(hr)) return hr;

	hr = pDevice->Activate(
		__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
	return hr;
}

HRESULT WasapiRenderer::CheckMediaType(const CMediaType* pMediaType)
{
	if (pMediaType == NULL) return E_POINTER;
	if (pMediaType->FormatType() == NULL) return E_POINTER;
	if (*pMediaType->FormatType() != FORMAT_WaveFormatEx)
		return VFW_E_TYPE_NOT_ACCEPTED;
	if (pMediaType->Format() == NULL) return E_POINTER;
	WAVEFORMATEX* pwf = (WAVEFORMATEX*)pMediaType->Format();
	if (m_pFormat == NULL) return IsFormatSupported(pwf);
	else return wfcmp(pwf, m_pFormat) ? VFW_E_TYPE_NOT_ACCEPTED : S_OK;
}

HRESULT WasapiRenderer::DoRenderSample(IMediaSample* pMediaSample)
{
	CAutoLock lock(&m_WorkerLock);
	m_Samples.push(pMediaSample);
	return S_OK;
}

HRESULT WasapiRenderer::Initialize(LPCWSTR pwstrId)
{
	if (pwstrId == NULL) return E_POINTER;
	if (m_pAudioClient != NULL) return VFW_E_WRONG_STATE;

	m_ShareMode = AUDCLNT_SHAREMODE_SHARED;

	HRESULT hr = Activate(pwstrId);
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT WasapiRenderer::Initialize(LPCWSTR pwstrId, const WAVEFORMATEX* pwf,
	AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED)
{
	if (pwstrId == NULL || pwf == NULL) return E_POINTER;
	if (m_pAudioClient != NULL) return VFW_E_WRONG_STATE;

	m_ShareMode = shareMode;

	HRESULT hr = Activate(pwstrId);
	if (FAILED(hr)) return hr;

	HRESULT hr = IsFormatSupported(pwf);
	if (FAILED(hr)) return hr;

	m_pFormat = wfcpy(pwf);
	if (m_pFormat == NULL) return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT WasapiRenderer::Initialize2()
{
	if (m_pRenderClient != NULL) return S_FALSE;

	if (m_pFormat == NULL)
	{
		AM_MEDIA_TYPE mt;
		GetPin(0)->ConnectionMediaType(&mt);
		m_pFormat = (WAVEFORMATEX*)mt.pbFormat;
	}

	HRESULT hr = m_pAudioClient->Initialize(m_ShareMode,
		AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, m_pFormat, NULL);
	if (FAILED(hr)) return hr;

	hr = m_pAudioClient->GetService(
		__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);
	if (FAILED(hr)) return hr;

	m_evBufferReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_evBufferReady == NULL) return HRESULT_FROM_WIN32(GetLastError());

	hr = m_pAudioClient->SetEventHandle(m_evBufferReady);
	if (FAILED(hr)) return  hr;

	return S_OK;
}

HRESULT WasapiRenderer::IsFormatSupported(const WAVEFORMATEX* pwf)
{
	WAVEFORMATEX* pClosestMatch = NULL;

	HRESULT hr = m_pAudioClient->IsFormatSupported(
		m_ShareMode, pwf, &pClosestMatch);
	if (pClosestMatch != NULL) CoTaskMemFree(pClosestMatch);
	if (hr == S_OK || FAILED(hr) && hr != AUDCLNT_E_UNSUPPORTED_FORMAT)
		return hr;

	WAVEFORMATEX* pwfalt = wfalt(pwf);
	if (pwfalt == NULL) return E_OUTOFMEMORY;

	hr = m_pAudioClient->IsFormatSupported(
		m_ShareMode, pwfalt, &pClosestMatch);
	if (pClosestMatch != NULL) CoTaskMemFree(pClosestMatch);
	CoTaskMemFree(pwfalt);

	if (hr != S_FALSE && hr != AUDCLNT_E_UNSUPPORTED_FORMAT) return hr;
	else return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT WasapiRenderer::OnStartStreaming()
{
	HRESULT hr = Initialize2();
	if (FAILED(hr)) return hr;

	if (!Create()) return E_FAIL;

	hr = m_pAudioClient->Start();
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT WasapiRenderer::OnStopStreaming()
{
	BOOL eventSet = SetEvent(m_evBufferReady);
	HRESULT hr = m_pAudioClient->Stop();
	if (eventSet) Close();
	return eventSet ? hr : HRESULT_FROM_WIN32(GetLastError());
}

HRESULT WasapiRenderer::ShouldDrawSampleNow(IMediaSample* pMediaSample,
	REFERENCE_TIME* pStartTime, REFERENCE_TIME* pEndTime)
{
	return S_OK;
}

DWORD WasapiRenderer::ThreadProc()
{
	while (true)
	{
		if (WaitForSingleObject(m_evBufferReady, INFINITE) == WAIT_FAILED)
			return HRESULT_FROM_WIN32(GetLastError());

		if (!m_bStreaming) break;

		UINT32 nBufferFrames;
		HRESULT hr = m_pAudioClient->GetBufferSize(&nBufferFrames);
		if (FAILED(hr)) return hr;
		
		BYTE* pbWrite;
		hr = m_pRenderClient->GetBuffer(nBufferFrames, &pbWrite);
		if (FAILED(hr)) return hr;

		if (m_pCurrentSample == NULL && m_Samples.empty())
		{
			hr = m_pRenderClient->ReleaseBuffer(
				nBufferFrames, AUDCLNT_BUFFERFLAGS_SILENT);
			if (FAILED(hr)) return hr;
			continue;
		}

		UINT32 iWrite = 0;

		while (true)
		{
			BYTE* pbRead;
			m_pCurrentSample->GetPointer(&pbRead);


		}

		// TODO: Render audio.
	}

	return S_OK;
}