#pragma once
#include "AudioClient.h"

namespace DirectShow
{
	class WasapiSource : CSource
	{
	public:
		WasapiSource(WASAPI::AudioClient&, HRESULT* phr);
	private:
		class Pin : CSourceStream
		{
		public:
			Pin(CSource* pms, WASAPI::AudioClient& audioClient, HRESULT* phr);
			HRESULT CheckMediaType(const CMediaType* mediaType);
			HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest);
			HRESULT FillBuffer(IMediaSample *pSample);
			HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
		private:
			WASAPI::AudioClient audioClient;
		};
		Pin pin;
	};
}