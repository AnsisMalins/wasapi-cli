#pragma once

namespace DirectShow
{
	using namespace std;

	class WasapiRenderer : public CAMThread, public CBaseRenderer
	{
	public:
		WasapiRenderer(HRESULT* phr);
		~WasapiRenderer();
		HRESULT CheckMediaType(const CMediaType* pmt);
		HRESULT DoRenderSample(IMediaSample* pMediaSample);
		HRESULT Initialize(LPCWSTR pwstrId);
		HRESULT Initialize(LPCWSTR pwstrId, const WAVEFORMATEX* pFormat,
			AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED);
		HRESULT OnStartStreaming();
		HRESULT OnStopStreaming();
		HRESULT ShouldDrawSampleNow(IMediaSample *pMediaSample,
			REFERENCE_TIME *pStartTime, REFERENCE_TIME *pEndTime);
		DWORD ThreadProc();
	private:
		HRESULT Activate(LPCWSTR pwstrId);
		HRESULT Initialize2();
		HRESULT IsFormatSupported(const WAVEFORMATEX* pwf);
		UINT32 m_iCurrentFrame;
		HANDLE m_evBufferReady;
		CComPtr<IAudioClient> m_pAudioClient;
		CComPtr<IMediaSample> m_pCurrentSample;
		WAVEFORMATEX* m_pFormat;
		CComPtr<IAudioRenderClient> m_pRenderClient;
		AUDCLNT_SHAREMODE m_ShareMode;
		queue<CComPtr<IMediaSample>> m_Samples;
	};
}