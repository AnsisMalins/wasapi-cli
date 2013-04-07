
/*++

    Copyright (c)  Microsoft Corporation.  All Rights Reserved.

    Module Name:

        projpch.h

    Abstract:

        Common include for all components hosted in the AX file.

    Notes:

--*/


#ifndef __projpch_h
#define __projpch_h

#if(_WIN32_WINNT < 0x0400)
#undef  _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

// (Build Compatibility for VC6/PSDK/VC.NET)
//
// To allow a clean build with standard Visual C++ 6.0 headers
// (without benefit of an updated Platform SDK), define SOCKET
// to prevent compilation errors within <winsock2.h>.
typedef unsigned int    SOCKET;

// Include before Windows.h to prevent inclusion of Winsock.h
#include <winsock2.h>   


#include <streams.h>
#include <tchar.h>

#include <nspapi.h>

#pragma warning( push )

// C4995'function': name was marked as #pragma deprecated
//
// The version of ws2tcpip.h which shipped with Visual Studio .NET 
// 2003 uses some deprecated functions.  Warning C4995 is disabled 
// because the file cannot be changed and we do not want to 
// display warnings which the user cannot fix.
#pragma warning( disable : 4995 )

#include <ws2tcpip.h>

#pragma warning( pop )

#define LOOPBACK_IFC                "Loopback\0"
#define ANY_IFC                     "any\0"
#define UNDEFINED_STR               "undefined\0"

//  combo boxes don't like to have -1 data associated with them, so we flag with the
//   next most unlikely value, I think ..
#define UNDEFINED                   ((-1) & ~1)

#define MAX_UDP_PAYLOAD             (65536 - 20 - 8)
#define MAX_IOBUFFER_LENGTH         8192
#define TS_PACKET_LENGTH            188

#define RELEASE_AND_CLEAR(p)        if (p) { (p) -> Release () ; (p) = NULL ; }
#define DELETE_RESET(p)             { delete (p) ; (p) = NULL ; }

template <class T> T Min (T a, T b) { return (a < b ? a : b) ; }

#endif  __projpch_h


#include <commctrl.h>
#include "nutil.h"

#define NET_RECV(pf)            (reinterpret_cast <CNetworkReceiverFilter *> (pf))