#pragma once
#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0600
#include <atlbase.h>
#include <audioclient.h>
#include <exception>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <stdio.h>
#include <sstream>
#include <string>

#define _STR(x) #x
#define STR(x) _STR(x)