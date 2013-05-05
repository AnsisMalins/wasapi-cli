#pragma once

namespace DirectShow
{
	class RtpRenderer : public CBaseRenderer
	{
	public:
		RtpRenderer(IN_ADDR localIP, SOCKADDR_IN remoteEP, HRESULT* phr);
		HRESULT CheckMediaType(const CMediaType* pMediaType);
		HRESULT DoRenderSample(IMediaSample* pMediaSample);
		HRESULT OnStartStreaming();
		HRESULT OnStopStreaming();
	private:
		IN_ADDR m_LocalIP;
		u_short m_nPacketsSent;
		SOCKADDR_IN m_RemoteEP;
		SOCKET m_Socket;
		u_long m_Ssid;
	};
}