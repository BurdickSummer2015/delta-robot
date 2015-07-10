/* Copyright (c) 2012, 3Dconnexion All rights reserved.
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


#include "StdAfx.h"
#include "3DxGalileoDialog.h"

/*| ------------------------------- Serial Port COMx ---------------------------------------------- |*/
// See "3DX-Sensor (GALILEO) Product Specification" section "4.1.1 Data interface (UART) settings:"

SerialPortConfiguration COMx = 
{ 
  "COM1", sizeof(COMx.ReceiveBuffer), sizeof(COMx.TransmitBuffer), EV_RXCHAR, 
  { sizeof(DCB), CBR_38400, TRUE, FALSE, FALSE,FALSE, DTR_CONTROL_DISABLE, FALSE, FALSE, FALSE,FALSE, TRUE,FALSE, RTS_CONTROL_DISABLE, FALSE, 0,0, 0,0, 8,NOPARITY,ONESTOPBIT, -1,-1,-1,-1,(char)COMMAND_END, 0 },
  { -1, 0,0, 0,0 },
  INVALID_HANDLE_VALUE, NULL
}; 
/*| ------------------------------- Serial Port COMx ---------------------------------------------- |*/


void EnableDisableGalileoDialogButtons( HWND dialog, bool enable )
{
HWND item;

 item = GetDlgItem( dialog, IDC_AUTO_START ); assert( item != NULL );
 Button_Enable( item, enable );
 
 item = GetDlgItem( dialog, IDC_AUTO_STOP ); assert( item != NULL );
 Button_Enable( item, enable );
 
 item = GetDlgItem( dialog, IDC_SINGLE ); assert( item != NULL );
 Button_Enable( item, enable );
 
 item = GetDlgItem( dialog, IDC_SETZERO ); assert( item != NULL );
 Button_Enable( item, enable );
 
 item = GetDlgItem( dialog, IDC_OPEN_SERIAL_PORT ); assert( item != NULL );
 Button_Enable( item, !enable );
 
 item = GetDlgItem( dialog, IDC_CLOSE_SERIAL_PORT ); assert( item != NULL );
 Button_Enable( item, enable );
}

void SetSerialPortNameGalileoDialog( HWND dialog, const char *name )
{
HWND item;
BOOL status;

 item = GetDlgItem( dialog, IDC_NAME_SERIAL_PORT ); assert( item != NULL );
 status = SetWindowText( item, name );
}

void GetSerialPortNameGalileoDialog( HWND dialog, char *name, int size )
{
HWND item;
BOOL status;

 ZeroMemory( name, size );
 item = GetDlgItem( dialog, IDC_NAME_SERIAL_PORT ); assert( item != NULL );
 status = GetWindowText( item, name, size );
}

void SetGalileoSensorErrors( HWND dialog, int checksum, int protocol )
{
char buffer[256];
HWND item;
BOOL status;

 item = GetDlgItem( dialog, IDC_CHECKSUM_ERRORS ); assert( item != NULL );
 sprintf_s( buffer, sizeof(buffer), "%d", checksum );
 status = SetWindowText( item, buffer ); 

 item = GetDlgItem( dialog, IDC_PROTOCOL_ERRORS ); assert( item != NULL );
 sprintf_s( buffer, sizeof(buffer), "%d", protocol );
 status = SetWindowText( item, buffer ); 
}

HBRUSH SolidBrush = (HBRUSH)NULL;

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
HWND dialog;
PAINTSTRUCT ps;
HDC hdc;
int status;
const COLORREF color = RGB( 64,64,255 ); 

 switch( uMsg )
  {
   case WM_INITDIALOG :
    EnableDisableGalileoDialogButtons( hwndDlg, false );
    SetSerialPortNameGalileoDialog( hwndDlg, COMx.name );
    SolidBrush = CreateSolidBrush( color );
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
    dialog = GetDlgItem( hwndDlg, LOWORD(wParam) ); 
    switch( LOWORD(wParam) )
     {
      case IDC_OPEN_SERIAL_PORT :
       GetSerialPortNameGalileoDialog( hwndDlg, COMx.name, sizeof(COMx.name) );
       if ( OpenSerialPort( COMx.name, &COMx ) == (HANDLE)INVALID_HANDLE_VALUE )
        {
         status = MessageBox( hwndDlg, "Serial Port does not exist or is already in use!", "Serial Port Failure", MB_OK );
        }
       else
        {
         EnableDisableGalileoDialogButtons( hwndDlg, true );
        };
       break;
       
      case IDC_CLOSE_SERIAL_PORT :
       if ( COMx.SerialPort != (HANDLE)INVALID_HANDLE_VALUE )
        CloseSerialPort( &COMx );
       COMx.SerialPort = (HANDLE)INVALID_HANDLE_VALUE;
       
       EnableDisableGalileoDialogButtons( hwndDlg, false );
       break;
       
      case IDC_AUTO_START :
       GalileoSensorAutoStart();
       break;
       
      case IDC_AUTO_STOP :
       GalileoSensorAutoStop();
       break;
       
      case IDC_SINGLE :
       GalileoSensorSingle();
       break;
       
      case IDC_SETZERO :
       GalileoSensorSetZero();
       break;
     };
    break;
   
   case WM_GALILEO_SENSOR_3D_XA :
    SetGalileoSensorDataSlider( hwndDlg, IDC_SENSOR_VALUE_X, IDC_SENSOR_SLIDER_X, wParam );
    SetGalileoSensorDataSlider( hwndDlg, IDC_SENSOR_VALUE_A, IDC_SENSOR_SLIDER_A, lParam );
    return TRUE;
   
   case WM_GALILEO_SENSOR_3D_YB :
    SetGalileoSensorDataSlider( hwndDlg, IDC_SENSOR_VALUE_Y, IDC_SENSOR_SLIDER_Y, wParam );
    SetGalileoSensorDataSlider( hwndDlg, IDC_SENSOR_VALUE_B, IDC_SENSOR_SLIDER_B, lParam );
    return TRUE;
   
   case WM_GALILEO_SENSOR_3D_ZC :
    SetGalileoSensorDataSlider( hwndDlg, IDC_SENSOR_VALUE_Z, IDC_SENSOR_SLIDER_Z, wParam );
    SetGalileoSensorDataSlider( hwndDlg, IDC_SENSOR_VALUE_C, IDC_SENSOR_SLIDER_C, lParam );
    return TRUE;
   
   case WM_GALILEO_SENSOR_ERROR :
    SetGalileoSensorErrors( hwndDlg, wParam, lParam );
    return TRUE;
  };

 return FALSE;
}

