#pragma once

#ifdef _DEBUG
#define DEBUG
#endif

// Faster compilation
#define WIN32_LEAN_AND_MEAN

// Windows Vista
#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#define WINVER 0x0600

// Base Classes
#include <streams.h>

// COM
#include <atlbase.h>

// DirectShow
#include <dshow.h>

// Standard Library
#include <cassert>
#include <csignal>
#include <exception>
#include <iostream>
#include <locale>
#include <map>
#include <queue>
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
