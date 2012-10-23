#include "StdAfx.h"
#include "CoInitializer.h"
#include "com_exception.h"

CoInitializer::CoInitializer(DWORD dwCoInit)
{
	HR(CoInitializeEx(NULL, COINIT_MULTITHREADED));
}

CoInitializer::~CoInitializer()
{
	CoUninitialize();
}