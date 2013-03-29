#pragma once

namespace WASAPI
{
	class AudioClient
	{
	public:
		AudioClient(IAudioClient* ptr, DWORD params);
		WAVEFORMATEX GetMixFormat() const;
		bool IsFormatSupported(const WAVEFORMATEX& format);
	private:
		CComPtr<IAudioClient> audioClient;
	};
}