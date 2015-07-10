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

/*| ---

Demo related to the document:

3DX-Sensor (GALILEO) Product Specification
GPS-3DX-400041 Revision: 001M
Revision 001 09/29/2011

    --- |*/
#include "StdAfx.h"
#include "3DxGalileoSensor.h"

unsigned int GalileoSensorChecksumError = 0;	// checksum error counter ...
unsigned int GalileoSensorProtocolError = 0;	// protocol error counter ...

unsigned int GalileoSensorReceiveBufferIndex = 0;	// fill level of the receive buffer ...
unsigned char GalileoSensorReceiveBuffer[256];	// receive buffer ...

bool GalileoSensorReadData( void ) // Read out any charcters from the serial port ... all valid commands documented at "4.2 Protocol" ...
{
signed int values[6+1];
BOOL result;
unsigned int start;
bool loop;

 assert( GalileoSensorReceiveBufferIndex < sizeof(GalileoSensorReceiveBuffer) ); // receive buffer should not be full ...
 GalileoSensorReceiveBufferIndex += ReadSerialPort( &COMx, &GalileoSensorReceiveBuffer[ GalileoSensorReceiveBufferIndex ], sizeof(GalileoSensorReceiveBuffer)-GalileoSensorReceiveBufferIndex ); // fill the receive buffer with all available characters from the serial port ...

 start = 0; loop = true; 
 
 while ( (GalileoSensorReceiveBufferIndex > 0) && loop )
  switch( GalileoSensorReceiveBuffer[start] )
   {
    case REQUEST_DATA_START :			// acknowledge for the REQUEST_DATA command, documented at "4.2.2 3DX-Sensor commands": ...
     if ( GalileoSensorReceiveBufferIndex >= REQUEST_DATA_START_LENGTH ) // acknowledge includes 2 characters ...
      {
       if ( !GalileoSensorGet3DData( &GalileoSensorReceiveBuffer[start], values ) ) // analyse the command and get out the X,Y,Z and A,B,C values, as well the checksum 
        {
         ++GalileoSensorChecksumError;   // count checksum errors and report them to the user interface ...
         result = PostMessage( GalileoSensorDialogWindow, WM_GALILEO_SENSOR_ERROR, GalileoSensorChecksumError, GalileoSensorProtocolError );
        };
        
       // report all values to the user interface ... 
       result = PostMessage( GalileoSensorDialogWindow, WM_GALILEO_SENSOR_3D_XA, values[0], values[3] );      
       result = PostMessage( GalileoSensorDialogWindow, WM_GALILEO_SENSOR_3D_YB, values[1], values[4] );
       result = PostMessage( GalileoSensorDialogWindow, WM_GALILEO_SENSOR_3D_ZC, values[2], values[5] );

       GalileoSensorReceiveBufferIndex -= REQUEST_DATA_START_LENGTH; // remove command from the buffer and 
       start += REQUEST_DATA_START_LENGTH;							 // analyse the next command ...
      }
     else
      loop = false;		// command is not complete, read the remaining command the next time ...
     break;
  
    case SET_ZERO_POSITION : // acknowledge for the SET_ZERO command, documented at "4.2.2 3DX-Sensor commands" ...
    case AUTO_DATA_ON :		 // acknowledge for the AUTO_DATA_ON command 
    case AUTO_DATA_OFF :	 // acknowledge for the AUTO_DATA_OFF command 
     if ( GalileoSensorReceiveBufferIndex >= COMMAND_LENGTH ) // acknowledge includes 2 characters ...
      {
       if ( GalileoSensorReceiveBuffer[start+1] != COMMAND_END ) // second character is 0x8D 
        ++GalileoSensorProtocolError;
      
       GalileoSensorReceiveBufferIndex -= COMMAND_LENGTH; // remove command from the buffer and 
       start += COMMAND_LENGTH;							  // analyse the next command ...
      }
     else
      loop = false;		// command is not complete, read the remaining command the next time ...
     break;
  
    default : // invalid response ...
     ++GalileoSensorProtocolError;  // really bad, a invalid response occured ...
     GalileoSensorReceiveBufferIndex -= 1; // remove just the single character ...
     start += 1;
     break;
   };
   
 if ( GalileoSensorReceiveBufferIndex > 0 )   // copy the remaining characters to the top of the receive buffer ...
  memcpy( GalileoSensorReceiveBuffer, &GalileoSensorReceiveBuffer[start], GalileoSensorReceiveBufferIndex );
 
 return GalileoSensorReceiveBufferIndex == 0;	// check for pending characters ...
}

bool GalileoSensorGet3DData( unsigned char *packet, signed int values[6+1] ) // analyse the 3D data command and save the X,Y,Z and A,B,C values, as well the checksum value ...
{
unsigned int loop;
signed checksum;

 if ( packet[0] != REQUEST_DATA_START ) return false;	// check for valid start and end command characters ...
 if ( packet[15] != COMMAND_END ) return false;

 for ( loop=0; loop<(6+1); ++loop )				// analyse 6+1 values, documented at "4.2.2 3DX-Sensor commands: REQUEST_DATA" ...
  {
   values[loop] = packet[loop*2+1] & 0x007F;	// connect two charcters to the valid value ...
   values[loop] <<= 7;
   values[loop] |= packet[loop*2+2] & 0x007F;
   
   values[loop] += loop == 6 ? 0 : -8192;		// the values X,Y,Z and A,B,C include an offset of +8192 for remove, checksum does not include an offset, documented at "4.2.2 3DX-Sensor commands: REQUEST_DATA" ...
  };

 checksum = 0;
 for ( loop=0; loop<13; ++loop )				// calculate the checksum from the command characters ...
  checksum += packet[loop];
 checksum &= 0x3FFF;

 return checksum == values[6];					// for a valid command both values must be equal ...
}

void GalileoSensorAutoStart( void )
{
unsigned char command = AUTO_DATA_ON;
DWORD size;

 size = WriteSerialPort( &COMx, &command, sizeof(command) ); // transmit the single character 0xAE for auto data on, documented at "4.2.2 3DX-Sensor commands: AUTO_DATA_ON" ...
}

void GalileoSensorAutoStop( void )
{
unsigned char command = AUTO_DATA_OFF;
DWORD size;

 size = WriteSerialPort( &COMx, &command, sizeof(command) );  // transmit the single character 0xAF for auto data off, documented at "4.2.2 3DX-Sensor commands: AUTO_DATA_OFF" ...
}

void GalileoSensorSingle( void )
{
unsigned char command = REQUEST_DATA;
DWORD size;

 size = WriteSerialPort( &COMx, &command, sizeof(command) ); // transmit the single character 0xAC for request data, documented at "4.2.2 3DX-Sensor commands: REQUEST_DATA" ...
}

void GalileoSensorSetZero( void )
{
unsigned char command = SET_ZERO_POSITION;	
DWORD size;

 size = WriteSerialPort( &COMx, &command, sizeof(command) ); // transmit the single character 0xAD for set zero position, documented at "4.2.2 3DX-Sensor commands: SET_ZERO_POSITION" ...
}

