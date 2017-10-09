// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#ifndef _WINDOWS_
//#define WIN32_LEAN_AND_MEAN
//#include<Windows.h>
//#undef WIN32_LEAN_AND_MEAN
//#endif

#include <WinSock2.h>
#include<Windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}


// TODO: reference additional headers your program requires here
