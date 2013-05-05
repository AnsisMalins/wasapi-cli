#pragma once

namespace DirectShow
{
	class RtpSource : public CSource
	{
	public:
		RtpSource(SOCKADDR_IN localEP, HRESULT* phr);
	private:
		class Pin : public CSourceStream
		{
		public:
			Pin(CSource* pms, SOCKADDR_IN localEP, HRESULT* phr);
			HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest);
			HRESULT Stop();
		protected:
			HRESULT FillBuffer(IMediaSample* pSample);
			HRESULT GetMediaType(CMediaType* pMediaType);
			HRESULT OnThreadStartPlay();
		private:
			LONGLONG m_llMediaTime;
			SOCKADDR_IN m_localEP;
			UINT m_nPacketsReceived;
			REFERENCE_TIME m_rtTime;
			SOCKET m_Socket;
		};
		Pin m_Pin;
	};
}