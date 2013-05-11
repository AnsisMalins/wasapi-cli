#pragma once

namespace DirectShow
{
	class StdoutRenderer : public CBaseRenderer
	{
	public:
		StdoutRenderer(HRESULT* phr);
		HRESULT CheckMediaType(const CMediaType* pmt);
		HRESULT DoRenderSample(IMediaSample* pMediaSample);
		HRESULT OnStartStreaming();
		HRESULT OnStopStreaming();
	private:
		int m_StdoutMode;
	};
}