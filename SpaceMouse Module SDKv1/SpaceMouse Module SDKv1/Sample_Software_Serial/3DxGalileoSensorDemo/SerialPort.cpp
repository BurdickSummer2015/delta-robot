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
#include "SerialPort.h"

                                 
HANDLE InvalidSerialPort( HANDLE handle )
{
 if ( handle != INVALID_HANDLE_VALUE )
  CloseHandle( handle );
 return INVALID_HANDLE_VALUE;
}

HANDLE OpenSerialPort( char *name, SerialPortConfiguration *config )
{
HANDLE handle;
BOOL result;
char *eventname;
size_t length;

 InitializeCriticalSection( &config->ReceiveTransmitBufferSection );
 config->ReceiveBufferLength = config->TransmitBufferLength = 0;
  
 config->SerialPort = INVALID_HANDLE_VALUE;
 
 handle = CreateFile( name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0,0 );
 if ( handle == INVALID_HANDLE_VALUE )
  return INVALID_HANDLE_VALUE;

 result = SetupComm( handle, config->InQueueSize, config->OutQueueSize );
 if ( result == false )
  return InvalidSerialPort( handle );

 memset( &config->Properties, 0x00, sizeof(config->Properties) );
 result = GetCommProperties( handle, &config->Properties );
 if ( result == false )
  return InvalidSerialPort( handle );
  
 if ( config->Parameters.DCBlength == 0 )
  { 
   config->Parameters.DCBlength = sizeof(config->Parameters); 
   result = GetCommState( handle, &config->Parameters );
   if ( result == false )
    return InvalidSerialPort( handle );
  }
 else
  {
   result = SetCommState( handle, &config->Parameters );
   if ( result == false )
    return InvalidSerialPort( handle );
  };

 result = SetCommMask( handle, config->EventMask );
 if ( result == false )
  return InvalidSerialPort( handle );

 result = SetCommTimeouts( handle, &config->Timeouts );
 if ( result == false )
  return InvalidSerialPort( handle );

 length = strlen(name) + 32;
 eventname = new char[ length ];
 if ( eventname == (char *)NULL )
  return InvalidSerialPort( handle );
 strcpy_s( eventname, length, name );
 strcat_s( eventname, length, "Event" );
 delete [] eventname; eventname = (char *)NULL;
  
 result = EscapeCommFunction( config->SerialPort, SETRTS );
 result = EscapeCommFunction( config->SerialPort, SETDTR );
 result = EscapeCommFunction( config->SerialPort, SETXON );
 
 config->SerialPort = handle;
 return handle;
}

void CloseSerialPort( SerialPortConfiguration *config )
{
 InvalidSerialPort( config->SerialPort );
 DeleteCriticalSection( &config->ReceiveTransmitBufferSection );
}

DWORD WriteSerialPort ( SerialPortConfiguration *config, unsigned char *buffer, DWORD length )
{
BOOL status;
DWORD result;

 result = length;
 while ( length-- > 0 )
  {
   status = TransmitCommChar( config->SerialPort, *buffer++ );
   assert( status );
  };

 return result;
}

DWORD ReadSerialPort( SerialPortConfiguration *config, unsigned char *buffer, DWORD length )
{
DWORD result;
BOOL status;

 result = 0;
 status = ReadFile( config->SerialPort, buffer, length, &result, NULL );
 return result;
}

