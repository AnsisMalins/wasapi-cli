#pragma once

class WaveFormat
{
public:
	WaveFormat(WORD wFormatTag, WORD nChannels,
		WORD wBitsPerSample, DWORD nSamplesPerSec);
	WaveFormat(const WAVEFORMATEX& wf);
	WaveFormat(const WAVEFORMATEXTENSIBLE& wf);
	DWORD AvgBytesPerSec() const;
	WORD BitsPerSample() const;
	WORD BlockAlign() const;
	WORD Channels() const;
	WORD Format() const;
	DWORD SamplesPerSec() const;
	operator WAVEFORMATEX() const;
	operator WAVEFORMATEXTENSIBLE() const;
	bool operator ==(const WaveFormat& wf) const;
	bool operator !=(const WaveFormat& wf) const;
private:
	DWORD data;
};