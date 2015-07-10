// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
// Windows-Headerdateien:
#include <windows.h>
#include <windowsx.h>

// C RunTime-Headerdateien
#include <stdlib.h>
#include <malloc.h>
#include  <memory.h>
#include <tchar.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"

#define DIRECTINPUT_VERSION 0x0800

#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#pragma warning(push)
#pragma warning(disable:6000 28251)
#define INITGUID
#include <Guiddef.h>
#include <dinput.h>
#pragma warning(pop)

#include <dinputd.h>
#include <oleauto.h>

