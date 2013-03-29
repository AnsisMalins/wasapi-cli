#include "StdAfx.h"
#include "CoInitializer.h"
#include "com_exception.h"

using namespace COM;

CoInitializer::CoInitializer(DWORD dwCoInit)
{
	HR(CoInitializeEx(NULL, dwCoInit));
}

CoInitializer::~CoInitializer()
{
	CoUninitialize();
}