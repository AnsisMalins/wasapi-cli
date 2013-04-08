#include "StdAfx.h"
#include "CoInitializer.h"
#include "com_exception.h"

using namespace COM;

CoInitializer::CoInitializer(DWORD dwCoInit)
{
	EX(CoInitializeEx(NULL, dwCoInit));
}

CoInitializer::CoInitializer(const CoInitializer& co)
{
}

CoInitializer::~CoInitializer()
{
	CoUninitialize();
}