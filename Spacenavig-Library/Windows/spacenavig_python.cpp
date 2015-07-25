// HIDTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <basetyps.h>
//#include <stdlib.h>
#include <wtypes.h>
//#include <guiddef.h>
#include <setupapi.h>
extern "C" {
#include <hidsdi.h>
}
#include <malloc.h>
#include <time.h>
#include "spacenavig_python.hpp"
#include <iostream>
#include <tchar.h>
//#include <stdio.h>


#define MAX_DEVICES 10
int nDevices = 0;
float trans[] = { 0.0, 0.0, 0.0 };
typedef struct _HidDevice {
	PSP_DEVICE_INTERFACE_DETAIL_DATA  pDevInterfaceDetailData;
	HANDLE handle;
	PHIDP_PREPARSED_DATA pPreparsedData;
	HIDP_CAPS capabilities;
	PHIDP_BUTTON_CAPS pInButCaps,pOutButCaps,pFeatButCaps;
	PHIDP_VALUE_CAPS pInValCaps,pOutValCaps,pFeatValCaps;
	char *buf;
	BOOL bGotTranslationVector, bGotRotationVector;
	int allDOFs[6];
} HidDevice;
HidDevice Devices[MAX_DEVICES];

HANDLE *waitListHandles;
HidDevice **waitListItems;
int waitListIndex = 0;
OVERLAPPED ol = { 0 };
DWORD g_BytesTransferred = 7;

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped)
{
	//_tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	//_tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	g_BytesTransferred = dwNumberOfBytesTransfered;
	//g_BytesTransferred = 7;
	//printf("BYTES: %x", dwNumberOfBytesTransfered);

}


double prevTime;
double zeroTime = .01;

SPACENAV_PYTHON_API void getMousePosition(float *mylist){
	mylist[0] = trans[0];
	mylist[1] = trans[1];
	mylist[2] = trans[2];
}



SPACENAV_PYTHON_API void readEvent(double inputTime){
	

	DWORD nread, waitResult;
	// This only wakes for one of the HID devices.  Not sure why.
	while(1){
		waitResult = WaitForMultipleObjects(waitListIndex, waitListHandles, FALSE, 0);
		//printf("WAIT: %i\n", waitResult);
		

		// printf("waitResult is %d\n",waitResult);

		// a HID device event
		if (waitResult < WAIT_OBJECT_0 + waitListIndex)
		{
			int index = waitResult - WAIT_OBJECT_0;
			HidDevice *pDev = waitListItems[index];
			//COMMTIMEOUTS timeouts;
			//std::cout << GetCommTimeouts(pDev->handle, &timeouts);
			//timeouts.ReadIntervalTimeout = 0;
			//timeouts.ReadTotalTimeoutMultiplier = 0;
			//timeouts.ReadTotalTimeoutConstant = 0;
			/*timeouts.WriteTotalTimeoutMultiplier = 1;
			timeouts.WriteTotalTimeoutConstant = 1;*/

			//LPCOMMTIMEOUTS lptimeouts = &timeouts;
			//std::cout << SetCommTimeouts(pDev->handle, lptimeouts);
			//std::cout << GetLastError();
			//printf("COMM TIMOUTS, %s",)/*;*/

			//DWORD bytesAvailable;
			//LPDWORD ptrbytesav = LPDWORD(&bytesAvailable);
			//PeekNamedPipe(pDev->handle, pDev->buf, 8, NULL, NULL, ptrbytesav);
			//printf("READ RESULT: %i\n", bytesAvailable);

			//WARNING: THIS IS A HACKISH IMPLEMENTATION
			//Essentially the loop will exit as soon as readResult takes time to execute
			//std::cout << "SLEEP" << std::endl;
			double elapse = (double)clock() / (double)CLOCKS_PER_SEC;
			//if (FALSE == ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine))
			//ULONG nword;
			//ol = (ULONG_PTR)&nword;
			nread = 0;
			int readResult = ReadFileEx(pDev->handle,
				pDev->buf,
				pDev->capabilities.InputReportByteLength,
				&ol,
				FileIOCompletionRoutine);
			
			DWORD sleepResult = SleepEx(0, TRUE);
			
			if (sleepResult && readResult){
				
				//elapse = ((double)clock() / (double)CLOCKS_PER_SEC) - elapse;
			
				/*std::cout << elapse << std::endl;
			
				std::cout << ol.Internal << std::endl;
				std::cout << pDev->buf[0] << std::endl;*/
				nread = g_BytesTransferred;
			
				/*if (readResult)
				{*/
					/*
					printf("%d bytes read:\n",nread);
					for (DWORD i=0; i<nread;i++)
					printf("%.2x ",pDev->pBuf[i] & 0x00ff);
					printf("\n");
					*/
					//printf("Char %i, %i\n", pDev->buf[0], nread);
					if (inputTime != 0){
						prevTime = inputTime;
					}
					else{
						prevTime = (double)clock() / (double)CLOCKS_PER_SEC;
					}

					switch (pDev->buf[0])
					{
					case 0x01:
						if (nread != 7) break; // something is wrong
						//printf("Device %d Got Translation vector\n", index);
						pDev->bGotTranslationVector = TRUE;
						pDev->allDOFs[0] = (pDev->buf[1] & 0x000000ff) | ((int)pDev->buf[2] << 8 & 0xffffff00);
						pDev->allDOFs[1] = (pDev->buf[3] & 0x000000ff) | ((int)pDev->buf[4] << 8 & 0xffffff00);
						pDev->allDOFs[2] = (pDev->buf[5] & 0x000000ff) | ((int)pDev->buf[6] << 8 & 0xffffff00);
						trans[0] = pDev->allDOFs[0];
						trans[1] = pDev->allDOFs[1];
						trans[2] = pDev->allDOFs[2];
						break;

					case 0x02:
						if (nread != 7) break; // something is wrong
						//printf("Device %d Got Rotation vector\n", index);
						pDev->bGotRotationVector = TRUE;
						pDev->allDOFs[3] = (pDev->buf[1] & 0x000000ff) | ((int)pDev->buf[2] << 8 & 0xffffff00);
						pDev->allDOFs[4] = (pDev->buf[3] & 0x000000ff) | ((int)pDev->buf[4] << 8 & 0xffffff00);
						pDev->allDOFs[5] = (pDev->buf[5] & 0x000000ff) | ((int)pDev->buf[6] << 8 & 0xffffff00);
						break;

					case 0x03:  // Buttons (display most significant byte to least)
						printf("Device %d Button mask: %.2x %.2x %.2x\n", index, (unsigned char)pDev->buf[3], (unsigned char)pDev->buf[2], (unsigned char)pDev->buf[1]);
						switch (pDev->buf[1])
						{
						case 0x01:  // Stop reading on button 1
							//bRun = FALSE;
							break;
						default:
							break;
						}
						break;

					default:
						break;
					}

					// Translation and Rotation values come in two different packets.  Wait until we have both of them.
					if (pDev->bGotTranslationVector && pDev->bGotRotationVector)
					{
						pDev->bGotTranslationVector = pDev->bGotRotationVector = FALSE;
		#if !COMPILING_DLL
						printf("Device %d, all 6 DOF: T: (%d, %d, %d) R: (%d, %d, %d)\n", index,
							pDev->allDOFs[0], pDev->allDOFs[1], pDev->allDOFs[2], pDev->allDOFs[3], pDev->allDOFs[4], pDev->allDOFs[5]);
		#endif
					}

		#if 0
					DWORD err = GetLastError();
					printf("GetLastError = 0x%x\n", err);
					char errbuf[128];
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, errbuf, sizeof(errbuf), NULL);
					printf("%s\n", errbuf);
		#endif

				//}
			}else{
				CancelIoEx(pDev->handle,&ol);
				break;
			}
			//if(elapse > .001){
			//	//WARNING: THIS IS A HACKISH IMPLEMENTATION
			//	//The proper implementation involving setting Commtimeouts was not working
			//	break;
			//}
		}else{
			break;
		}
		
	}
	//printf("END\n");
	double c;
	if (inputTime != 0){
		c = inputTime;
	}
	else{
		c = (double)clock();
	}

	if (c > (double)(prevTime + zeroTime)){
		trans[0] = 0;
		trans[1] = 0;
		trans[2] = 0;
	}


}

SPACENAV_PYTHON_API int init(){
	GUID hidGuid;



	HidD_GetHidGuid(&hidGuid);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL,
		DIGCF_DEVICEINTERFACE | DIGCF_PRESENT | DIGCF_PROFILE);

	printf("Known HID Devices:\n<none>\r");
	int memberIndex = 0;
	SP_DEVICE_INTERFACE_DATA  DevInterfaceData;
	DevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	while (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &hidGuid, memberIndex, &DevInterfaceData))
	{
		DWORD requiredSize;

		// First call, just gets the size so we can malloc enough memory for the detail struct
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevInterfaceData, NULL, 0, &requiredSize, NULL);

		// Second call gets the data
		Devices[nDevices].pDevInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
		Devices[nDevices].pDevInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		SetupDiGetDeviceInterfaceDetail(hDevInfo,
			&DevInterfaceData,
			Devices[nDevices].pDevInterfaceDetailData,
			requiredSize,
			NULL,
			NULL);
		//printf("DevicePath: %s\n", Devices[nDevices].pDevInterfaceDetailData->DevicePath);

		// Open this device
		Devices[nDevices].handle = CreateFile(Devices[nDevices].pDevInterfaceDetailData->DevicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL);
		if (Devices[nDevices].handle == NULL)
		{
			printf("Error opening file \"%s\"\n", Devices[nDevices].pDevInterfaceDetailData->DevicePath);
			goto next;
		}

		// Get the preparsedData struct
		if (HidD_GetPreparsedData(Devices[nDevices].handle, &Devices[nDevices].pPreparsedData) != TRUE)
		{
			printf("Error getting PreparsedData\n");
			goto next;
		}

		if (HidP_GetCaps(Devices[nDevices].pPreparsedData, &Devices[nDevices].capabilities) != HIDP_STATUS_SUCCESS)
		{
			printf("Error getting capabilities\n");
			goto next;
		}
		
	next:
		memberIndex++;
		if (++nDevices == MAX_DEVICES) { printf("overflow\n"); --nDevices; }
	} // end of while(SetupDiEnumDeviceInterfaces) loop

	// Done with devInfo list.  Release it.
	//SetupDiDestroyDeviceInfoList( hDevInfo );

	//while(1){}

	// Read data from each usagePage==1, usage==8 device
	int listIndex = 0;
	waitListHandles = (HANDLE *)malloc(nDevices * sizeof(HANDLE));
	waitListItems = (HidDevice **)malloc(nDevices * sizeof(HidDevice*));
	for (listIndex = 0; listIndex < nDevices; listIndex++)
	{
		if (Devices[listIndex].capabilities.UsagePage == 1 && Devices[listIndex].capabilities.Usage == 8)
		{
			// Add its handle and a pointer to the waitList
			waitListHandles[waitListIndex] = Devices[listIndex].handle;
			HidDevice *pDev = Devices + listIndex;
			waitListItems[waitListIndex] = pDev;
			pDev->buf = (char *)malloc(pDev->capabilities.InputReportByteLength);
			ZeroMemory(pDev->buf, pDev->capabilities.InputReportByteLength);
			pDev->bGotTranslationVector =
				pDev->bGotRotationVector = FALSE;
			waitListIndex++;

		}
	}
	
	printf("Connected to %i SpaceNav 3D Mouses...", waitListIndex);
	return waitListIndex;

}

SPACENAV_PYTHON_API void close(){
	for (int i = 0; i<nDevices; i++)
	{
		// Free the preparsedData
		if (Devices[i].pPreparsedData)
			HidD_FreePreparsedData(Devices[i].pPreparsedData);

		// Close handles
		if (Devices[i].handle != INVALID_HANDLE_VALUE)
			CloseHandle(Devices[i].handle);

		// Free pDevInterfaceDetailData
		if (Devices[i].pDevInterfaceDetailData)
			free(Devices[i].pDevInterfaceDetailData);

		if (Devices[i].buf)
			free(Devices[i].buf);
	}
}

int main(int argc, char* argv[])
{
	printf("\n%s\n\n", argv[0]);

	if (init()){
		// print out all received events
		clock_t t = clock();
		while (1){
			if (clock() > t + CLOCKS_PER_SEC /30){
				readEvent(((double)t / (double)CLOCKS_PER_SEC));
				t = clock();
			}
		}
	}

	return 0;
}








