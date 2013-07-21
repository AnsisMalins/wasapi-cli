#pragma once
#include "wexception.h"

class sock_exception : public wexception
{
public:
	sock_exception(const wchar_t* message, int error);
	int error() const;
private:
	int _error;
};

int throw_on_socket_error(int socket_error, const wchar_t* context);