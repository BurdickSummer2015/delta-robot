// stdafx.h : Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
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
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"

// TODO: Hier auf zus�tzliche Header, die das Programm erfordert, verweisen.
#include "SerialPort.h"
#include "3DxGalileoSensorDemo.h"
#include "3DxGalileoDialog.h"
#include "3DxGalileoSensor.h"
