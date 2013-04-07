#include "StdAfx.h"
#include "CoInitializer.h"
#include "wexception.h"

using namespace COM;

CoInitializer::CoInitializer(DWORD dwCoInit)
{
	EX(CoInitializeEx(NULL, dwCoInit));
}

CoInitializer::~CoInitializer()
{
	CoUninitialize();
}