/* Copyright (c) 2014, 3Dconnexion All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of 3Dconnexion may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY 3DCONNEXION ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL 3DCONNEXION BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdafx.h"

extern LPDIRECTINPUT8          g_pDI;
extern LPDIRECTINPUTDEVICE8    g_pJoystick;

#include "3DxSMM_USB.h"

HBRUSH SolidBrush = (HBRUSH)NULL;


void DisableDialogItems(HWND dialog)
{
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_VALUE_X), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_SLIDER_X), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_VALUE_Y), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_SLIDER_Y), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_VALUE_Z), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_SLIDER_Z), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_VALUE_A), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_SLIDER_A), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_VALUE_B), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_SLIDER_B), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_VALUE_C), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_SENSOR_SLIDER_C), FALSE);
}


void SetGalileoSensorDataSlider( HWND dialog, int idcvalue, int idcslider, int value )
{
const int minvalue = -350;
const int maxvalue = +350;
PAINTSTRUCT ps;
char buffer[256];
BOOL status;
HWND item;
double scale;
LONG limit0, limit;
RECT rectangle;

 value = value > maxvalue ? maxvalue :
         value < minvalue ? minvalue : value;
         
 item = GetDlgItem( dialog, idcvalue ); assert( item != NULL );
 sprintf_s( buffer, sizeof(buffer), "%+d", value );
 status = SetWindowText( item, buffer ); 

 item = GetDlgItem( dialog, idcslider ); assert( item != NULL );
 ps.hdc = GetDC( item );
 status = GetClientRect( item, &ps.rcPaint );
 
 scale = (double)(0 - minvalue) / (double)(maxvalue - minvalue);
 limit0 = (LONG)(( ps.rcPaint.right - ps.rcPaint.left )*scale) + ps.rcPaint.left;
    
 scale = (double)(value - minvalue) / (double)(maxvalue - minvalue);
 limit = (LONG)(( ps.rcPaint.right - ps.rcPaint.left )*scale) + ps.rcPaint.left;
        
 rectangle.left = limit0 < limit ? limit0 : limit;
 rectangle.right = (limit0 < limit ? limit : limit0) + 1;
 rectangle.top = ps.rcPaint.top;
 rectangle.bottom = ps.rcPaint.bottom;
 FillRect( ps.hdc, &rectangle, SolidBrush );

 rectangle.left = 0;
 rectangle.right = (limit0 < limit ? limit0 : limit) - 1;
 rectangle.top = ps.rcPaint.top;
 rectangle.bottom = ps.rcPaint.bottom;
 if ( rectangle.left < rectangle.right )
  FillRect( ps.hdc, &rectangle, (HBRUSH)COLOR_WINDOW );
    
 rectangle.left = (limit0 > limit ? limit0 : limit) + 1;
 rectangle.right = ps.rcPaint.right+1;
 rectangle.top = ps.rcPaint.top;
 rectangle.bottom = ps.rcPaint.bottom;
 if ( rectangle.left < rectangle.right )
  FillRect( ps.hdc, &rectangle, (HBRUSH)COLOR_WINDOW );

 ReleaseDC( item, ps.hdc );    
}

INT_PTR CALLBACK GalileoDialogCallback( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
PAINTSTRUCT ps;
HDC hdc;
const COLORREF color = RGB( 64,64,255 ); 

 switch( uMsg )
  {
   case WM_INITDIALOG :

	DisableDialogItems(hwndDlg);
	SolidBrush = CreateSolidBrush( color );
	if (FAILED(InitDirectInput(hwndDlg)))
	{
		SendMessage(hwndDlg, WM_SYSCOMMAND, SC_CLOSE, 0);
	}

	// Set a timer to go off 30 times a second. At every timer message
	// the input device will be read
	SetTimer(hwndDlg, 0, 1000 / 30, NULL);

	return TRUE;
 
   case WM_PAINT :
	hdc = BeginPaint(hwndDlg, &ps);
	EndPaint(hwndDlg, &ps);
    break;    
 
   case WM_SYSCOMMAND :
    if ( wParam == SC_CLOSE )
     {
      DeleteObject( SolidBrush );
      EndDialog( hwndDlg, 0 );
      PostQuitMessage(0);
     };
    break;
 
   case WM_COMMAND :
    break;

   case WM_ACTIVATE:
	   if (WA_INACTIVE != wParam && g_pJoystick)
	   {
		   // Make sure the device is acquired, if we are gaining focus.
		   g_pJoystick->Acquire();
	   }
	   return TRUE;

   case WM_TIMER:
	   // Update the input device every timer message
	   if (FAILED(UpdateInputState(hwndDlg)))
	   {
		   KillTimer(hwndDlg, 0);
		   MessageBox(NULL, TEXT("Error Reading Joystick Data.\n ") \
			   TEXT("The sample will now exit."), TEXT("SpaceMouseModule USB"),
			   MB_ICONERROR | MB_OK);
		   SendMessage(hwndDlg, WM_SYSCOMMAND, SC_CLOSE, 0);
	   }
	   return TRUE;
 
  };

 return FALSE;
}

