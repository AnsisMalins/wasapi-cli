#include "stdafx.h"
#include "StdoutRenderer.h"

using namespace DirectShow;
using namespace std;

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

	char* pBuffer;
	HRESULT hr = pMediaSample->GetPointer((BYTE**)&pBuffer);
	if (hr != S_OK) return hr;

	long length = pMediaSample->GetActualDataLength();
	if (length < 0) return E_INVALIDARG;

	try { cout.write(pBuffer, length); }
	catch (const exception&) { return E_FAIL; }

	return S_OK;
}