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

	long length = pMediaSample->GetActualDataLength();
	if (length < 0) return E_INVALIDARG;

	if ((long)fwrite(pBuffer, 1, length, stdout) < length) return E_FAIL;

	return S_OK;
}

HRESULT StdoutRenderer::OnStartStreaming()
{
	if (fflush(stdout) != 0) return E_FAIL;
	stdoutMode = _setmode(_fileno(stdout), _O_BINARY);
	if (stdoutMode == -1) return E_FAIL;
	return S_OK;
}

HRESULT StdoutRenderer::OnStopStreaming()
{
	if (fflush(stdout) != 0) return E_FAIL;
	if (_setmode(_fileno(stdout), stdoutMode) == -1) return E_FAIL;
	return S_OK;
}