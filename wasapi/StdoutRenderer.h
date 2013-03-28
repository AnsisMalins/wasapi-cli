#pragma once

namespace DirectShow
{
	class StdoutRenderer : public CBaseRenderer
	{
	public:
		StdoutRenderer(HRESULT* phr);
		HRESULT CheckMediaType(const CMediaType* pmt);
		HRESULT DoRenderSample(IMediaSample* pMediaSample);
	};
}