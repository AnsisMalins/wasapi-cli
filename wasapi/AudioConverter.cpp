#include "stdafx.h"
#include "AudioConverter.h"

using namespace DirectShow;

AudioConverter::AudioConverter(const WAVEFORMATEX& wfOut, HRESULT* phr) :
	CTransformFilter(NAME("AudioConverter"), NULL, GUID_NULL)
{
	m_mtOut.InitMediaType();
	m_mtOut.SetType(&MEDIATYPE_Audio);
	m_mtOut.SetSubtype(&MEDIASUBTYPE_PCM);
	m_mtOut.SetFormatType(&FORMAT_WaveFormatEx);
	if (wfOut.wFormatTag == WAVE_FORMAT_UNKNOWN) return;

	if (wfOut.nChannels < 1
		|| wfOut.wBitsPerSample % 8 > 0 || wfOut.wBitsPerSample > 32
		|| wfOut.nBlockAlign != wfOut.wBitsPerSample / 8 * wfOut.nChannels
		|| wfOut.nAvgBytesPerSec != wfOut.nBlockAlign * wfOut.nSamplesPerSec)
	{
		if (phr != NULL) *phr = E_INVALIDARG;
		return;
	}
	if (wfOut.wFormatTag != WAVE_FORMAT_PCM
		&& wfOut.wFormatTag != WAVE_FORMAT_IEEE_FLOAT)
	{
		if (phr != NULL) *phr = VFW_E_TYPE_NOT_ACCEPTED;
		return;
	}
	m_mtOut.SetFormat((BYTE*)&wfOut, sizeof(wfOut));
	m_mtOut.SetSampleSize(wfOut.nBlockAlign);
}

HRESULT AudioConverter::CheckInputType(const CMediaType* mtIn)
{
	if (mtIn == NULL) return E_POINTER;

	if (mtIn->FormatType() == NULL) return E_POINTER;
	if (*mtIn->FormatType() != FORMAT_WaveFormatEx)
		return VFW_E_TYPE_NOT_ACCEPTED;

	if (mtIn->Format() == NULL) return E_POINTER;
	WAVEFORMATEXTENSIBLE* pwfIn = (WAVEFORMATEXTENSIBLE*)mtIn->Format();

	if (pwfIn->Format.wFormatTag != WAVE_FORMAT_PCM
		&& pwfIn->Format.wFormatTag != WAVE_FORMAT_IEEE_FLOAT
		&& pwfIn->Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE)
		return VFW_E_TYPE_NOT_ACCEPTED;

	if (pwfIn->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE
		&& pwfIn->SubFormat != KSDATAFORMAT_SUBTYPE_PCM
		&& pwfIn->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		return VFW_E_TYPE_NOT_ACCEPTED;

	return S_OK;
}

HRESULT AudioConverter::CheckTransform(
	const CMediaType* mtIn, const CMediaType* mtOut)
{
	if (mtIn == NULL || mtIn->Format() == NULL
		|| mtOut == NULL || mtOut->Format() == NULL) return E_POINTER;
	
	WAVEFORMATEX* pwfIn = (WAVEFORMATEX*)mtIn->Format();
	WAVEFORMATEX* pwfOut = (WAVEFORMATEX*)mtOut->Format();
	if (pwfIn->nSamplesPerSec != pwfOut->nSamplesPerSec
		|| pwfIn->nChannels != pwfOut->nChannels)
		return VFW_E_TYPE_NOT_ACCEPTED;

	m_mtIn = *mtIn;
	m_mtOut = *mtOut;

	WAVEFORMATEXTENSIBLE* pwfIn2 = (WAVEFORMATEXTENSIBLE*)m_mtIn.Format();
	if (pwfIn2->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		if (pwfIn2->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
			pwfIn2->Format.wFormatTag = WAVE_FORMAT_PCM;
		else
			pwfIn2->Format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	}
		
	return S_OK;
}

HRESULT AudioConverter::DecideBufferSize(
	IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest)
{
	if (pAlloc == NULL || ppropInputRequest == NULL) return E_POINTER;

	CComPtr<IMemAllocator> pAllocIn;
	HRESULT hr = m_pInput->GetAllocator(&pAllocIn);
	if (FAILED(hr)) return hr;
	
	ALLOCATOR_PROPERTIES apIn;
	hr = pAllocIn->GetProperties(&apIn);
	if (FAILED(hr)) return hr;

	long cbNeed = apIn.cbBuffer / m_mtIn.lSampleSize * m_mtOut.lSampleSize;
	ppropInputRequest->cbBuffer = max(ppropInputRequest->cbBuffer, cbNeed);
	ppropInputRequest->cBuffers = max(ppropInputRequest->cBuffers, 2);

	ALLOCATOR_PROPERTIES apActual;
	hr = pAlloc->SetProperties(ppropInputRequest, &apActual);
	if (FAILED(hr)) return hr;

	if (apActual.cbBuffer < cbNeed || apActual.cBuffers < 2) return E_FAIL;

	return S_OK;
}

HRESULT AudioConverter::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	if (iPosition < 0) return E_INVALIDARG;
	if (pMediaType == NULL) return E_POINTER;
	if (iPosition > 0) return VFW_S_NO_MORE_ITEMS;
	*pMediaType = m_mtOut;
	return S_OK;
}

HRESULT AudioConverter::Transform(IMediaSample* pIn, IMediaSample* pOut)
{
	if (pIn == NULL || pOut == NULL) return E_POINTER;

	WAVEFORMATEX* pwfIn = (WAVEFORMATEX*)m_mtIn.pbFormat;
	WAVEFORMATEX* pwfOut = (WAVEFORMATEX*)m_mtOut.pbFormat;
	long cbIn = pIn->GetActualDataLength();
	long cbOut = pOut->GetSize();

	if (cbIn / pwfIn->nBlockAlign * pwfOut->nBlockAlign > cbOut)
		return VFW_E_BUFFER_OVERFLOW;

	BYTE* pbIn;
	HRESULT hr = pIn->GetPointer(&pbIn);
	if (FAILED(hr)) return hr;

	BYTE* pbOut;
	hr = pOut->GetPointer(&pbOut);
	if (FAILED(hr)) return hr;

	BYTE* pbRead = pbIn;
	BYTE* pbWrite = pbOut;
	BYTE* pbEnd = pbIn + cbIn;
	while (pbRead < pbEnd)
	{
		float f;
		if (pwfIn->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
		{
			f = *(float*)pbRead;
			pbRead += 4;
		}
		else if (pwfIn->wBitsPerSample == 8)
		{
			f = (*pbRead - 128) / 127.0f;
			pbRead += 1;
		}
		else if (pwfIn->wBitsPerSample == 16)
		{
			f = *(short*)pbRead / 32767.0f;
			pbRead += 2;
		}
		else if (pwfIn->wBitsPerSample == 32)
		{
			f = *(int*)pbRead / 2147483647.0f;
			pbRead += 4;
		}
		f = min(max(f, -1.0f), 1.0f);
		if (pwfOut->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
		{
			*(float*)pbWrite = f;
			pbWrite += 4;
		}
		else if (pwfOut->wBitsPerSample == 8)
		{
			*pbWrite = (BYTE)(f * 127.0f + 128);
			pbWrite += 1;
		}
		else if (pwfOut->wBitsPerSample == 16)
		{
			*(short*)pbWrite = (short)(f * 32767.0f);
			pbWrite += 2;
		}
		else if (pwfOut->wBitsPerSample == 32)
		{
			*(int*)pbWrite = (int)(f * 2147483647.0f);
			pbWrite += 4;
		}
	}

	pOut->SetActualDataLength((long)(pbWrite - pbOut));

	return S_OK;
}