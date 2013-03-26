#pragma once

#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0600

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <atlbase.h>
#include <objbase.h>

#include <dshow.h>

#include <audioclient.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

#include <fcntl.h>
#include <io.h>

#define _STR(x) #x
#define STR(x) _STR(x)