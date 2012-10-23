#include "StdAfx.h"
#include "args_vector.h"

using namespace std;

args_vector::args_vector(int argc, wchar_t** argv)
{
	reserve(argc);
	for (int i = 0; i < argc; i++) push_back(wstring(argv[i]));
}