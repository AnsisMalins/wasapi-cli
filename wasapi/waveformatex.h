#pragma once

WAVEFORMATEX* wfalt(const WAVEFORMATEX* pwf);
int wfcmp(const WAVEFORMATEX* pwf1, const WAVEFORMATEX* pwf2);
WAVEFORMATEX* wfcpy(const WAVEFORMATEX* pwf);
WAVEFORMATEX* wfetowf(const WAVEFORMATEX* pwf);
WAVEFORMATEX* wfinit(WORD wFormatTag, WORD nChannels,
	WORD wBitsPerSample, DWORD nSamplesPerSec);
WAVEFORMATEX* wftowfe(const WAVEFORMATEX* pwf);