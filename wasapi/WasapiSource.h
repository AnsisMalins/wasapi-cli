#pragma once
#include "AudioClient.h"

namespace DirectShow
{
	class WasapiSource : public CSource
	{
	public:
		WasapiSource(LPCWSTR id, BOOL loopback, HRESULT* phr);
	private:
		class Pin : CSourceStream
		{
		public:
			Pin(CSource* pms, LPCWSTR id, BOOL loopback, HRESULT* phr);
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
			HANDLE m_hBufferReady;
			BOOL m_Initialized;
			BOOL m_Loopback;
			CComPtr<IAudioClient> m_pAudioClient;
			CComPtr<IAudioCaptureClient> m_pCaptureClient;
			CComPtr<IAudioClient> m_pEventClient;
			REFERENCE_TIME m_rtPrevious;
		};
		Pin m_Pin;
	};
}