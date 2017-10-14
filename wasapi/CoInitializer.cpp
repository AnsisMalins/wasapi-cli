#include "stdafx.h"
#include "CoInitializer.h"
#include "COMException.h"

CoInitializer::CoInitializer(DWORD dwCoInit)
{
	Check(CoInitializeEx(NULL, dwCoInit), __FILE__, __LINE__);
}

CoInitializer::~CoInitializer()
{
	CoUninitialize();
}
