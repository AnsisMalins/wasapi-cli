#pragma once

template<typename T>
class Const
{
public:
	Const(const T& t) : t(t) { }
	const T& operator *() { return t; }
	const T* operator ->() { return &t; }
private:
	const T t;
};