#include "stdafx.h"
#include "WaveFormat.h"

using namespace std;

WaveFormat::WaveFormat(WORD wFormatTag, WORD nChannels,
	WORD wBitsPerSample, DWORD nSamplesPerSec) :
	data(nSamplesPerSec << 8
		| wBitsPerSample / 8 - 1 << 6
		| wFormatTag >> 1 << 5
		| nChannels)
{
}

WaveFormat::WaveFormat(const WAVEFORMATEX& wf) :
	data(wf.nSamplesPerSec << 8
		| wf.wBitsPerSample / 8 - 1 << 6
		| wf.wFormatTag >> 1 << 5
		| wf.nChannels)
{
}

WaveFormat::WaveFormat(const WAVEFORMATEXTENSIBLE& wf) :
	data(wf.Format.nSamplesPerSec << 8
		| wf.Format.wBitsPerSample / 8 - 1 << 6
		| wf.Format.wFormatTag >> 1 << 5
		| wf.Format.nChannels)
{
}

DWORD WaveFormat::AvgBytesPerSec() const
{
	return BlockAlign() * SamplesPerSec();
}

WORD WaveFormat::BitsPerSample() const
{
	return (((data >> 6) & 0x3) + 1) * 8;
}

WORD WaveFormat::BlockAlign() const
{
	return BitsPerSample() / 8 * Channels();
}

WORD WaveFormat::Channels() const
{
	return data & 0x1f;
}

WORD WaveFormat::Format() const
{
	return data >> 5 << 1 | 1;
}

DWORD WaveFormat::SamplesPerSec() const
{
	return data >> 8;
}

WaveFormat::operator WAVEFORMATEX() const
{
	WAVEFORMATEX wf;
	wf.wFormatTag = Format();
	wf.nChannels = Channels();
	wf.wBitsPerSample = BitsPerSample();
	wf.nSamplesPerSec = SamplesPerSec();
	wf.nBlockAlign = BlockAlign();
	wf.nAvgBytesPerSec = AvgBytesPerSec();
	wf.cbSize = 0;
	return wf;
}

WaveFormat::operator WAVEFORMATEXTENSIBLE() const
{
	WAVEFORMATEXTENSIBLE wf;
	wf.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	wf.Format.nChannels = Channels();
	wf.Format.wBitsPerSample = BitsPerSample();
	wf.Format.nSamplesPerSec = SamplesPerSec();
	wf.Format.nBlockAlign = BlockAlign();
	wf.Format.nAvgBytesPerSec = AvgBytesPerSec();
	wf.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	wf.Samples.wReserved = 0;
	switch (Channels())
	{
	case 1:
		wf.dwChannelMask = SPEAKER_FRONT_CENTER;
		break;
	default:
		wf.dwChannelMask = (1 << Channels()) - 1;
		break;
	}
	switch (Format())
	{
	case WAVE_FORMAT_PCM:
		wf.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case WAVE_FORMAT_IEEE_FLOAT:
		wf.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		break;
	default:
		wf.SubFormat = GUID_NULL;
		break;
	}
	return wf;
}

bool WaveFormat::operator ==(const WaveFormat& wf) const
{
	return data == wf.data;
}

bool WaveFormat::operator !=(const WaveFormat& wf) const
{
	return data != wf.data;
}