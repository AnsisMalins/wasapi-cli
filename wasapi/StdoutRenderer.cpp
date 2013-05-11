#include "stdafx.h"
#include "StdoutRenderer.h"

using namespace DirectShow;

StdoutRenderer::StdoutRenderer(HRESULT* phr) :
	CBaseRenderer(GUID_NULL, NAME("StdoutRenderer"), NULL, phr)
{
}

HRESULT StdoutRenderer::CheckMediaType(const CMediaType* pmt)
{
	return pmt != NULL ? S_OK : E_POINTER;
}

HRESULT StdoutRenderer::DoRenderSample(IMediaSample* pMediaSample)
{
	if (pMediaSample == NULL) return E_POINTER;

	BYTE* pBuffer;
	HRESULT hr = pMediaSample->GetPointer(&pBuffer);
	if (hr != S_OK) return hr;

	long cbActual = pMediaSample->GetActualDataLength();

	if ((long)fwrite(pBuffer, 1, cbActual, stdout) < cbActual) return E_FAIL;

	return S_OK;
}

HRESULT StdoutRenderer::OnStartStreaming()
{
	if (fflush(stdout)) return E_FAIL;
	m_StdoutMode = _setmode(_fileno(stdout), _O_BINARY);
	if (m_StdoutMode == -1) return E_FAIL;
	return S_OK;
}

HRESULT StdoutRenderer::OnStopStreaming()
{
	if (fflush(stdout)) return E_FAIL;
	if (_setmode(_fileno(stdout), m_StdoutMode) == -1) return E_FAIL;
	return S_OK;
}