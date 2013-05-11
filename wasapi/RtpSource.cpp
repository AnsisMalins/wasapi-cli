#include "stdafx.h"
#include "RtpSource.h"

using namespace DirectShow;

RtpSource::RtpSource(SOCKADDR_IN localEP, HRESULT* phr) :
	CSource(NAME("RtpSource"), NULL, GUID_NULL, phr),
	m_Pin(this, localEP, phr)
{
}

RtpSource::Pin::Pin(CSource* pms, SOCKADDR_IN epLocal, HRESULT* phr) :
	CSourceStream(NAME("RtpSource::Pin"), phr, pms, L"1"),
	m_epLocal(epLocal)
{
	if (phr != NULL && FAILED(*phr)) return;

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		if (phr != NULL) *phr = E_UNEXPECTED;
		return;
	}
}

HRESULT RtpSource::Pin::DecideBufferSize(
	IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest)
{
	if (pAlloc == NULL || ppropInputRequest == NULL) return E_POINTER;

	ppropInputRequest->cbBuffer = max(ppropInputRequest->cbBuffer, 2048);
	ppropInputRequest->cBuffers = max(ppropInputRequest->cBuffers, 2);

	ALLOCATOR_PROPERTIES actual;
	HRESULT hr = pAlloc->SetProperties(ppropInputRequest, &actual);
	if (FAILED(hr)) return hr;

	if (actual.cbBuffer < 2048 || actual.cBuffers < 2) return E_FAIL;

	return S_OK;
}

HRESULT RtpSource::Pin::FillBuffer(IMediaSample* pSample)
{
	if (pSample == NULL) return E_POINTER;

	BYTE* pBuffer;
	HRESULT hr = pSample->GetPointer(&pBuffer);
	if (FAILED(hr)) return hr;

	hr = pSample->SetActualDataLength(0);
	if (FAILED(hr)) return hr;

	SOCKADDR_IN from;
	int fromlen = sizeof(SOCKADDR_IN);
	int cbActual = recvfrom(m_Socket,
		(char*)pBuffer, pSample->GetSize(), 0, (SOCKADDR*)&from, &fromlen);
	if (cbActual == SOCKET_ERROR)
	{
		int wserr = WSAGetLastError();
		switch (wserr)
		{
		case WSAEINTR: return S_FALSE;
		case WSAEMSGSIZE: return VFW_E_BUFFER_OVERFLOW;
		case WSAESHUTDOWN: return S_FALSE;
		default: return E_UNEXPECTED;
		}
	}

	int cbHeader = 12;
	if (cbActual < cbHeader) return E_FAIL;
	BYTE bVersion = (pBuffer[0] & 0xc0) >> 6;
	if (bVersion != 2) return E_FAIL;
	BOOL hasPadding = pBuffer[0] & 0x20;
	BOOL hasExtension = pBuffer[0] & 0x10;
	BYTE nContributingSources = pBuffer[0] & 0x0f;
	BOOL hasMarker = pBuffer[1] & 0x80;
	BYTE bPayloadType = pBuffer[1] & 0x7f;
	u_short iSeqNum = ntohs(*(u_short*)(pBuffer + 2));
	u_long ulTimestamp = ntohl(*(u_long*)(pBuffer + 4));
	u_long ulSyncSource = ntohl(*(u_long*)(pBuffer + 8));
	cbHeader += nContributingSources * 4;
	if (hasExtension)
	{
		cbHeader += 4;
		if (cbActual < cbHeader) return E_FAIL;
		int extensionID = ntohs(*(u_short*)(pBuffer + cbHeader - 4));
		int cbExtension = ntohs(*(u_short*)(pBuffer + cbHeader - 2));
		cbHeader += cbExtension;
	}
	if (hasPadding)
	{
		if (cbActual - 1 < cbHeader) return E_FAIL;
		int padding = pBuffer[cbActual - 1];
		cbActual -= padding;
	}
	if (cbActual < cbHeader) return E_FAIL;
	cbActual -= cbHeader;

	// Only 16 bit 44.1 kHz stereo supported for now.
	if (bPayloadType != 10) return VFW_E_UNSUPPORTED_STREAM;
	if (cbActual % 2 > 0) return E_FAIL;

	if (m_nPacketsReceived > 0
		&& iSeqNum <= m_iPrevSeqNum && iSeqNum + 100 > m_iPrevSeqNum)
		return S_OK;

	u_short* read = (u_short*)(pBuffer + cbHeader);
	u_short* write = (u_short*)pBuffer;
	u_short* end = (u_short*)(pBuffer + cbHeader + cbActual);
	for (; read < end; read++, write++) *write = ntohs(*read);

	hr = pSample->SetActualDataLength(cbActual);
	if (FAILED(hr)) return hr;

	// Not sure if this is the correct way to do this.
	LONGLONG llMediaTimeEnd = m_llMediaTime + cbActual / 4;
	hr = pSample->SetMediaTime(&m_llMediaTime, &llMediaTimeEnd);
	if (FAILED(hr)) return hr;

	if (m_nPacketsReceived > 0)
	{
		REFERENCE_TIME rtEnd = ulTimestamp;
		hr = pSample->SetTime(&m_rtTime, &rtEnd);
		if (FAILED(hr)) return hr;
	}

	m_iPrevSeqNum = iSeqNum;
	m_llMediaTime = llMediaTimeEnd;
	m_nPacketsReceived++;
	m_rtTime = ulTimestamp;

	return S_OK;
}

HRESULT RtpSource::Pin::GetMediaType(CMediaType* pMediaType)
{
	if (pMediaType == NULL) return E_POINTER;
	
	pMediaType->InitMediaType();
	pMediaType->SetType(&MEDIATYPE_Audio);
	pMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
	pMediaType->SetFormatType(&FORMAT_WaveFormatEx);

	pMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
	WAVEFORMATEX* pFormat = (WAVEFORMATEX*)pMediaType->Format();
	pFormat->wFormatTag = WAVE_FORMAT_PCM;
	pFormat->nChannels = 2;
	pFormat->nSamplesPerSec = 44100;
	pFormat->wBitsPerSample = 16;
	pFormat->nBlockAlign = pFormat->wBitsPerSample / 8 * pFormat->nChannels;
	pFormat->nAvgBytesPerSec = pFormat->nBlockAlign * pFormat->nSamplesPerSec;
	pFormat->cbSize = 0;

	pMediaType->SetSampleSize(pFormat->nBlockAlign);

	return S_OK;
}

HRESULT RtpSource::Pin::OnThreadStartPlay()
{
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_Socket == INVALID_SOCKET) return E_UNEXPECTED;

	char reuseaddr = 1;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR,
		&reuseaddr, sizeof(reuseaddr))) return E_UNEXPECTED;

	if (bind(m_Socket, (SOCKADDR*)&m_epLocal, sizeof(m_epLocal)))
		return E_FAIL;

	m_llMediaTime = 0;
	m_nPacketsReceived = 0;

	return S_OK;
}

HRESULT RtpSource::Pin::Stop()
{
	if (closesocket(m_Socket)) return E_UNEXPECTED;
	return CSourceStream::Stop();
}