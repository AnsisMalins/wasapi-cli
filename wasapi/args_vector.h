#pragma once

class args_vector : public std::vector<std::wstring>
{
public:
	args_vector(int argc, wchar_t** argv);
};