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

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

struct _SerialPortConfiguration_
{
 char name[MAX_PATH];
 /*| --- Input Parameters --- |*/
 DWORD InQueueSize, OutQueueSize, EventMask;
 DCB Parameters;
 COMMTIMEOUTS Timeouts;
 
 /*| --- Output Parameters --- |*/
 HANDLE SerialPort;
 COMMPROP Properties;
 
 /*| --- Windows and Buffers --- |*/
 HWND Window;
 CRITICAL_SECTION ReceiveTransmitBufferSection;
 unsigned char ReceiveBuffer[256];
 unsigned int ReceiveBufferLength;
 
 unsigned char TransmitBuffer[256];
 unsigned int TransmitBufferLength;
};
typedef struct _SerialPortConfiguration_ SerialPortConfiguration;

HANDLE OpenSerialPort( char *name, SerialPortConfiguration *config );
void CloseSerialPort( SerialPortConfiguration *config );

DWORD WriteSerialPort( SerialPortConfiguration *config, unsigned char *buffer, DWORD length );
DWORD ReadSerialPort( SerialPortConfiguration *config, unsigned char *buffer, DWORD length );

#endif /*| _SERIALPORT_H_ |*/
