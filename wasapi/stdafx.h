#pragma once

#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0600

//Base Classes
#include <streams.h>

// COM
#include <atlbase.h>

// DirectShow
#include <dshow.h>

// DSNetwork
#include <dsnetifc.h>
#include <dsrecv.h>
#include <dssend.h>

// STL
#include <csignal>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// WASAPI
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

// Windows IO
#include <fcntl.h>
#include <io.h>

#define _STR(x) #x
#define STR(x) _STR(x)
#define CONTEXT (__FILE__ ":" STR(__LINE__) " " )