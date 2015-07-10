// 3DxGalileoSensorDemo.cpp 
//
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

#include "stdafx.h"
#include "3DxGalileoSensorDemo.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			

ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HWND GalileoSensorDialogWindow = (HWND)NULL;

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
UNREFERENCED_PARAMETER(hPrevInstance);
UNREFERENCED_PARAMETER(lpCmdLine);

MSG msg;
HACCEL hAccelTable;
HWND application;
BOOL status;
DWORD result;

 LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
 LoadString(hInstance, IDC_MY3DXGALILEOSENSORDEMO, szWindowClass, MAX_LOADSTRING);
 MyRegisterClass(hInstance);

 application = InitInstance (hInstance, nCmdShow); assert( application != NULL );

 hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY3DXGALILEOSENSORDEMO));

 GalileoSensorDialogWindow = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_3DX_GALILEO_DIALOG), application, GalileoDialogCallback, (LPARAM)NULL ); assert( GalileoSensorDialogWindow != NULL );
 status = ShowWindow( GalileoSensorDialogWindow, SW_SHOW );

 do
  {
   if ( COMx.SerialPort == (HANDLE)INVALID_HANDLE_VALUE )
    result = GetMessage( &msg, NULL, 0,0 ) != -1;
   else
    {
     result = PeekMessage( &msg, NULL, 0,0, PM_REMOVE );
     if ( result == 0 )
      if ( GalileoSensorReadData() )
       Sleep(1);
    };
    
   if ( result == 1 )
    {
     if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
      {
 	   TranslateMessage(&msg);
	   DispatchMessage(&msg);
	   if ( msg.message == WM_QUIT )
	    break;
	  };
	};
  } while ( true );
  
 return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY3DXGALILEOSENSORDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY3DXGALILEOSENSORDEMO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
HWND hWnd;

 hInst = hInstance; 
 hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL); assert( hWnd != NULL );
 
 return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
PAINTSTRUCT ps;
HDC hdc;

 switch (message)
  {
   case WM_PAINT:
	hdc = BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
	break;
	
   case WM_DESTROY:
	PostQuitMessage(0);
	break;
	
   default:
	return DefWindowProc(hWnd, message, wParam, lParam);
  };

 return 0;
}

