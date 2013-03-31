#pragma once

namespace WASAPI
{
	class AudioClient
	{
	public:
		AudioClient(IAudioClient* ptr);
		void GetBuffer(BYTE** buffer, UINT32* frameCount);
		UINT32 GetBufferSize() const;
		UINT32 GetCurrentPadding() const;
		REFERENCE_TIME GetDevicePeriod() const;
		WAVEFORMATEX GetMixFormat() const;
		REFERENCE_TIME GetStreamLatency() const;
		void Initialize(DWORD params);
		bool IsFormatSupported(const WAVEFORMATEX& format);
		void ReleaseBuffer();
		void Reset();
		void SetEventHandle(HANDLE eventHandle);
		void Start();
		void Stop();
	private:
		CComPtr<IAudioClient> audioClient;
		UINT32 bufferSize;
		CComPtr<IAudioCaptureClient> captureClient;
		UINT32 frameSize;
		CComPtr<IAudioRenderClient> renderClient;
	};
}