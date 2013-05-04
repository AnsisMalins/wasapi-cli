#include "stdafx.h"
#include "RtpSource.h"

using namespace DirectShow;

RtpSource::RtpSource(SOCKADDR_IN localEP, HRESULT* phr) :
	CSource(NAME("RtpSource"), NULL, GUID_NULL, phr),
	m_Pin(this, localEP, phr)
{
}

RtpSource::Pin::Pin(CSource* pms, SOCKADDR_IN localEP, HRESULT* phr) :
	CSourceStream(NAME("RtpSource::Pin"), phr, pms, L"1"),
	m_llMediaTime(0),
	m_localEP(localEP),
	m_rtTime(0),
	m_Socket(NULL)
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata)) *phr = E_UNEXPECTED;
}

HRESULT RtpSource::Pin::DecideBufferSize(
	IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* ppropInputRequest)
{
	if (pAlloc == NULL || ppropInputRequest == NULL) return E_POINTER;

	ppropInputRequest->cbBuffer = max(ppropInputRequest->cbBuffer, 8992);
	ppropInputRequest->cBuffers = max(ppropInputRequest->cBuffers, 2);

	ALLOCATOR_PROPERTIES actual;
	HRESULT hr = pAlloc->SetProperties(ppropInputRequest, &actual);
	if (FAILED(hr)) return hr;

	if (actual.cbBuffer < 1492 || actual.cBuffers < 2) return E_FAIL;

	return S_OK;
}

HRESULT RtpSource::Pin::FillBuffer(IMediaSample* pSample)
{
	if (pSample == NULL) return E_POINTER;

	BYTE* pBuffer;
	HRESULT hr = pSample->GetPointer(&pBuffer);
	if (FAILED(hr)) return hr;

	long cbSize = pSample->GetSize();

	sockaddr from;
	int fromlen;
	long cbActual = recvfrom(m_Socket,
		(char*)pBuffer, cbSize, 0, &from, &fromlen);
	if (cbActual == SOCKET_ERROR)
	{
		pSample->SetActualDataLength(0);
		switch (WSAGetLastError())
		{
		case WSAEINTR: return S_FALSE;
		case WSAESHUTDOWN: return S_FALSE;
		case WSAEMSGSIZE: return VFW_E_BUFFER_OVERFLOW;
		default: return E_UNEXPECTED;
		}
	}

	int cbHeader = 12;
	if (cbActual < cbHeader) return E_FAIL;
	int version = pBuffer[0] & 0x3;
	if (version != 2) return E_FAIL;
	BOOL padding = pBuffer[0] & 0x4;
	BOOL extension = pBuffer[0] & 0x8;
	int nCsrc = pBuffer[0] >> 4;
	bool marker = pBuffer[1] & 0x1;
	int payloadType = pBuffer[1] >> 1;
	int sequenceNumber = ntohs(*(u_short*)(pBuffer + 2));
	u_long timestamp = ntohl(*(u_long*)(pBuffer + 4));
	u_long ssrc = ntohl(*(u_long*)(pBuffer + 8));
	cbHeader += nCsrc * 4;
	if (extension)
	{
		cbHeader += 4;
		if (cbActual < cbHeader) return E_FAIL;
		int extensionID = ntohs(*(u_short*)(pBuffer + cbHeader - 4));
		int cbExtension = ntohs(*(u_short*)(pBuffer + cbHeader - 2));
		cbHeader += cbExtension;
	}
	if (padding)
	{
		if (cbActual - 1 < cbHeader) return E_FAIL;
		int padding = pBuffer[cbActual - 1];
		cbActual -= padding;
	}
	if (cbActual < cbHeader) return E_FAIL;

	// Only 16 bit 44.1 kHz stereo supported for now.
	if (payloadType != 10) return VFW_E_UNSUPPORTED_STREAM;
	if (cbActual % 2 > 0) return E_FAIL;

	u_short* read = (u_short*)(pBuffer + cbHeader);
	u_short* write = (u_short*)pBuffer;
	u_short* end = (u_short*)(pBuffer + cbActual);
	for (; read < end; read++, write++) *write = ntohs(*read);

	cbActual -= cbHeader;

	hr = pSample->SetActualDataLength(cbActual);
	if (FAILED(hr)) return hr;

	// The following section causes a deadlock.
	/*LONGLONG llMediaTimeEnd = m_llMediaTime + cbActual / 4;
	hr = pSample->SetMediaTime(&m_llMediaTime, &llMediaTimeEnd);
	if (FAILED(hr)) return hr;

	REFERENCE_TIME rtEnd = timestamp;
	hr = pSample->SetTime(&m_rtTime, &rtEnd);
	if (FAILED(hr)) return hr;

	m_llMediaTime = llMediaTimeEnd;
	m_rtTime = rtEnd;*/

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

	if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR,
		(char*)TRUE, sizeof(TRUE))) return E_UNEXPECTED;

	if (bind(m_Socket, (SOCKADDR*)&m_localEP, sizeof(m_localEP)))
		return E_FAIL;

	return S_OK;
}

HRESULT RtpSource::Pin::Stop()
{
	if (closesocket(m_Socket)) return E_UNEXPECTED;
	return CSourceStream::Stop();
}