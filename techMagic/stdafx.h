// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream> 
#include <sstream>
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
