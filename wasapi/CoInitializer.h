#pragma once

class CoInitializer final
{
public:
	CoInitializer(DWORD dwCoInit);
	~CoInitializer();
    CoInitializer(const CoInitializer&) = delete;
    CoInitializer& operator =(const CoInitializer&) = delete;
};
