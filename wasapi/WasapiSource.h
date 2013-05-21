#pragma once
#include "AudioClient.h"

namespace DirectShow
{
	class WasapiSource : public CSource//, public IAMResourceControl
	{
	public:
		WasapiSource(HRESULT* phr, LPCWSTR id,
			AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED);
	private:
		class Pin : public CSourceStream//, public IAMStreamConfig 
		{
		public:
			Pin(HRESULT* phr, CSource* pms, LPCWSTR id, AUDCLNT_SHAREMODE shareMode);
			HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest);
		protected:
			HRESULT CheckMediaType(const CMediaType* pMediaType);
			HRESULT FillBuffer(IMediaSample* pSample);
			HRESULT GetMediaType(CMediaType* pMediaType);
			HRESULT OnThreadDestroy();
			HRESULT OnThreadStartPlay();
		private:
			HRESULT Initialize();
			UINT32 m_cbFrame;
			EDataFlow m_eDataFlow;
			HANDLE m_hBufferReady;
			BOOL m_Initialized;
			CComPtr<IAudioClient> m_pAudioClient;
			CComPtr<IAudioCaptureClient> m_pCaptureClient;
			CComPtr<IAudioClient> m_pEventClient;
			REFERENCE_TIME m_rtPrevious;
			AUDCLNT_SHAREMODE m_ShareMode;
		};
		Pin m_Pin;
	};
}