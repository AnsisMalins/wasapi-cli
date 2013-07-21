#include "stdafx.h"
#include "RtpRenderer.h"

using namespace DirectShow;

RtpRenderer::RtpRenderer(IN_ADDR localIP, SOCKADDR_IN remoteEP, HRESULT* phr) :
	CBaseRenderer(GUID_NULL, NAME("RtpRenderer"), NULL, phr),
	m_LocalIP(localIP),
	m_RemoteEP(remoteEP),
	m_Ssid(rand())
{
	if (phr != NULL && FAILED(*phr)) return;

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		if (phr != NULL) *phr = E_UNEXPECTED;
		return;
	}
}

HRESULT RtpRenderer::CheckMediaType(const CMediaType* pMediaType)
{
	if (pMediaType == NULL) return E_POINTER;
	if (pMediaType->FormatType() == NULL) return E_POINTER;
	if (*pMediaType->FormatType() != FORMAT_WaveFormatEx)
		return VFW_E_TYPE_NOT_ACCEPTED;
	if (pMediaType->Format() == NULL) return E_POINTER;
	WAVEFORMATEX* pFormat = (WAVEFORMATEX*)pMediaType->Format();
	if (pFormat->wFormatTag != WAVE_FORMAT_PCM) return VFW_E_TYPE_NOT_ACCEPTED;
	if (pFormat->nChannels != 2) return VFW_E_TYPE_NOT_ACCEPTED;
	if (pFormat->nSamplesPerSec != 44100) return VFW_E_TYPE_NOT_ACCEPTED;
	if (pFormat->wBitsPerSample != 16) return VFW_E_TYPE_NOT_ACCEPTED;
	return S_OK;
}

HRESULT RtpRenderer::DoRenderSample(IMediaSample* pMediaSample)
{
	if (pMediaSample == NULL) return E_POINTER;

	BYTE* pBuffer;
	HRESULT hr = pMediaSample->GetPointer(&pBuffer);
	if (FAILED(hr)) return hr;

	long cbActual = pMediaSample->GetActualDataLength();
	if (cbActual + 12 > pMediaSample->GetSize()) return E_FAIL;

	u_short* read = (u_short*)(pBuffer + cbActual - 2);
	u_short* write = (u_short*)(pBuffer + cbActual - 2 + 12);
	u_short* begin = (u_short*)pBuffer;
	for (; read >= begin; read--, write--) *write = htons(*read);
	cbActual += 12;

	REFERENCE_TIME rtTime;
	hr = pMediaSample->GetTime(NULL, &rtTime);
	if (FAILED(hr)) hr = pMediaSample->GetMediaTime(NULL, &rtTime);
	if (FAILED(hr)) rtTime = m_nPacketsSent;

	while (cbActual > 12)
	{
		*(u_short*)pBuffer = htons(0x800a);
		*(u_short*)(pBuffer + 2) = htons(m_nPacketsSent);
		*(u_long*)(pBuffer + 4) = htonl((u_long)rtTime);
		*(u_long*)(pBuffer + 8) = htonl(m_Ssid);

		int cbSent = sendto(m_Socket, (char*)pBuffer, min(cbActual, 1472), 0,
			(SOCKADDR*)&m_RemoteEP, sizeof(m_RemoteEP));
		if (cbSent == SOCKET_ERROR || cbSent < 12) return E_FAIL;
		pBuffer += cbSent - 12;
		cbActual -= cbSent - 12;

		m_nPacketsSent++;
	}

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