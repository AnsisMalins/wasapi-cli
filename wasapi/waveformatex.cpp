#include "stdafx.h"
#include "waveformatex.h"

WAVEFORMATEX* wfalt(const WAVEFORMATEX* pwf)
{
	WAVEFORMATEX* result = wftowfe(pwf);
	if (result == NULL) result = wfetowf(pwf);
	return result;
}

int wfcmp(const WAVEFORMATEX* pwf1, const WAVEFORMATEX* pwf2)
{
	if (pwf1 == NULL || pwf2 == NULL)
	{
		if (pwf1 == NULL) return -1;
		else if (pwf2 == NULL) return 1;
		else return 0;
	}

	WAVEFORMATEX* pwfe1 = NULL;
	WAVEFORMATEX* pwfe2 = NULL;
	if (pwf1->wFormatTag == WAVE_FORMAT_EXTENSIBLE
		|| pwf2->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		WAVEFORMATEX* pwfe1 = wftowfe(pwf1);
		if (pwfe1 != NULL) pwf1 = pwfe1;
		WAVEFORMATEX* pwfe2 = wftowfe(pwf2);
		if (pwfe2 != NULL) pwf2 = pwfe2;
	}

	int result = pwf2->cbSize - pwf1->cbSize;
	if (result == 0) result =
		memcmp(pwf1, pwf2, sizeof(WAVEFORMATEX) + pwf1->cbSize);

	if (pwfe1 != NULL) CoTaskMemFree(pwfe1);
	if (pwfe2 != NULL) CoTaskMemFree(pwfe2);

	return result;
}

WAVEFORMATEX* wfcpy(const WAVEFORMATEX* pwf)
{
	SIZE_T cbwf = sizeof(WAVEFORMATEX) + pwf->cbSize;
	LPVOID result = CoTaskMemAlloc(cbwf);
	if (result == NULL) return NULL;
	memcpy(result, pwf, cbwf);
	return (WAVEFORMATEX*)result;
}

WAVEFORMATEX* wfetowf(const WAVEFORMATEX* pwf)
{
	if (pwf == NULL || pwf->cbSize != 0
		|| pwf->wFormatTag != WAVE_FORMAT_PCM
		&& pwf->wFormatTag != WAVE_FORMAT_IEEE_FLOAT
		&& pwf->wFormatTag != WAVE_FORMAT_DRM
		&& pwf->wFormatTag != WAVE_FORMAT_ALAW
		&& pwf->wFormatTag != WAVE_FORMAT_MULAW
		&& pwf->wFormatTag != WAVE_FORMAT_ADPCM) return NULL;

	WAVEFORMATEXTENSIBLE* pwfe = (WAVEFORMATEXTENSIBLE*)
		CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
	if (pwfe == NULL) return NULL;

	memcpy(pwfe, pwf, sizeof(WAVEFORMATEX));

	switch (pwf->wFormatTag)
	{
	case WAVE_FORMAT_PCM:
		pwfe->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case WAVE_FORMAT_IEEE_FLOAT:
		pwfe->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		break;
	case WAVE_FORMAT_DRM:
		pwfe->SubFormat = KSDATAFORMAT_SUBTYPE_DRM;
		break;
	case WAVE_FORMAT_ALAW:
		pwfe->SubFormat = KSDATAFORMAT_SUBTYPE_ALAW;
		break;
	case WAVE_FORMAT_MULAW:
		pwfe->SubFormat = KSDATAFORMAT_SUBTYPE_MULAW;
		break;
	case WAVE_FORMAT_ADPCM:
		pwfe->SubFormat = KSDATAFORMAT_SUBTYPE_ADPCM;
	}

	pwfe->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	pwfe->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	if (pwf->nChannels == 1) pwfe->dwChannelMask = SPEAKER_FRONT_CENTER;
	else pwfe->dwChannelMask = (1 << pwf->nChannels) - 1;
	pwfe->Samples.wReserved = 0;

	return (WAVEFORMATEX*)pwfe;
}

WAVEFORMATEX* wfinit(WORD wFormatTag, WORD nChannels,
	WORD wBitsPerSample, DWORD nSamplesPerSec)
{
	if (wBitsPerSample % 8 > 0) return NULL;
	WAVEFORMATEX* pwf = (WAVEFORMATEX*)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
	if (pwf == NULL) return NULL;

	pwf->wFormatTag = wFormatTag;
	pwf->nChannels = nChannels;
	pwf->wBitsPerSample = wBitsPerSample;
	pwf->nSamplesPerSec = nSamplesPerSec;
	pwf->nBlockAlign = wBitsPerSample / 8 * nChannels;
	pwf->nAvgBytesPerSec = pwf->nBlockAlign * nSamplesPerSec;
	pwf->cbSize = 0;

	return pwf;
}

WAVEFORMATEX* wftowfe(const WAVEFORMATEX* pwf)
{
	if (pwf == NULL || pwf->wFormatTag != WAVE_FORMAT_EXTENSIBLE
		|| pwf->cbSize != sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))
		return NULL;
	WAVEFORMATEXTENSIBLE* pwfe = (WAVEFORMATEXTENSIBLE*)pwf;
	if (pwfe->SubFormat != KSDATAFORMAT_SUBTYPE_PCM
		&& pwfe->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT
		&& pwfe->SubFormat != KSDATAFORMAT_SUBTYPE_DRM
		&& pwfe->SubFormat != KSDATAFORMAT_SUBTYPE_ALAW
		&& pwfe->SubFormat != KSDATAFORMAT_SUBTYPE_MULAW
		&& pwfe->SubFormat != KSDATAFORMAT_SUBTYPE_ADPCM) return NULL;

	WAVEFORMATEX* pwf2 = (WAVEFORMATEX*)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
	if (pwf == NULL) return NULL;

	memcpy(pwf2, pwfe, sizeof(WAVEFORMATEX));

	if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
		pwf2->wFormatTag = WAVE_FORMAT_PCM;
	else if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		pwf2->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	else if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_DRM)
		pwf2->wFormatTag = WAVE_FORMAT_DRM;
	else if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_ALAW)
		pwf2->wFormatTag = WAVE_FORMAT_ALAW;
	else if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_MULAW)
		pwf2->wFormatTag = WAVE_FORMAT_MULAW;
	else if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_ADPCM)
		pwf2->wFormatTag = WAVE_FORMAT_ADPCM;

	pwf2->cbSize = 0;

	return pwf2;
}