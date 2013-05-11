#pragma once

namespace DirectShow
{
	class AudioConverter : public CTransformFilter
	{
	public:
		AudioConverter(const WAVEFORMATEX& format, HRESULT* phr);
		HRESULT CheckInputType(const CMediaType* mtIn);
		HRESULT CheckTransform(
			const CMediaType* mtIn, const CMediaType* mtOut);
		HRESULT DecideBufferSize(
			IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest);
		HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);
		HRESULT Transform(IMediaSample* pIn, IMediaSample* pOut);
	private:
		CMediaType m_mtIn;
		CMediaType m_mtOut;
	};
}