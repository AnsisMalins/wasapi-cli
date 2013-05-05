#include "stdafx.h"
#include "RtpRenderer.h"

using namespace DirectShow;

RtpRenderer::RtpRenderer(IN_ADDR localIP, SOCKADDR_IN remoteEP, HRESULT* phr) :
	CBaseRenderer(GUID_NULL, NAME("RtpRenderer"), NULL, phr),
	m_Ssid(rand())
{
}

HRESULT RtpRenderer::CheckMediaType(const CMediaType* pMediaType)
{
	if (pMediaType == NULL) return E_POINTER;
	if (pMediaType->FormatType() == NULL) return E_FAIL;
	if (*pMediaType->FormatType() != FORMAT_WaveFormatEx) return E_FAIL;
	if (pMediaType->Format() == NULL) return E_FAIL;
	WAVEFORMATEX* pFormat = (WAVEFORMATEX*)pMediaType->Format();
	if (pFormat->wFormatTag != WAVE_FORMAT_PCM) return E_FAIL;
	if (pFormat->nChannels != 2) return E_FAIL;
	if (pFormat->nSamplesPerSec != 44100) return E_FAIL;
	if (pFormat->wBitsPerSample != 16) return E_FAIL;
	return S_OK;
}

HRESULT RtpRenderer::DoRenderSample(IMediaSample* pMediaSample)
{
	if (pMediaSample == NULL) return E_POINTER;

	BYTE* pBuffer;
	HRESULT hr = pMediaSample->GetPointer(&pBuffer);
	if (FAILED(hr)) return hr;

	long cbSize = pMediaSample->GetSize();
	long cbActual = pMediaSample->GetActualDataLength();

	u_short* read = (u_short*)(pBuffer + cbActual - 2);
	u_short* write = (u_short*)(pBuffer + cbActual - 2 + 12);
	u_short* begin = (u_short*)pBuffer;
	for (; read >= begin; read--, write--) *write = htons(*read);

	REFERENCE_TIME rtTime;
	hr = pMediaSample->GetTime(NULL, &rtTime);
	if (FAILED(hr)) return E_UNEXPECTED;

	*(u_short*)pBuffer = 0x800a;
	*(u_short*)(pBuffer + 2) = htons(m_nPacketsSent);
	*(u_long*)(pBuffer + 4) = htonl((u_long)rtTime);
	*(u_long*)(pBuffer + 8) = m_Ssid;

	m_nPacketsSent++;

	return S_OK;
}

HRESULT RtpRenderer::OnStartStreaming()
{
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_Socket == INVALID_SOCKET) return E_UNEXPECTED;

	char reuseaddr = 1;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR,
		&reuseaddr, sizeof(reuseaddr))) return E_UNEXPECTED;

	SOCKADDR_IN localEP = { AF_INET, 0, m_LocalIP };
	if (bind(m_Socket, (SOCKADDR*)&localEP, sizeof(localEP)))
		return E_FAIL;

	m_nPacketsSent = 0;

	return S_OK;
}

HRESULT RtpRenderer::OnStopStreaming()
{
	if (closesocket(m_Socket)) return E_UNEXPECTED;
	return S_OK;
}