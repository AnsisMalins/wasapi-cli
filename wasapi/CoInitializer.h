#pragma once

namespace COM
{
	class CoInitializer
	{
	public:
		CoInitializer(DWORD dwCoInit);
		~CoInitializer();
	private:
		CoInitializer(const CoInitializer& co);
	};
}