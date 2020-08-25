//----------------------------------Defines--------------------------------
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define TIME_PORT 27015
#define ROME_DELTA (+2)	
#define WASHINGTOM_DELTA (-4)
#define BEIJING_DELTA (+8)	// Beijin
#define SYDNEY_DELTA (+10)
#define REQ_NUM 100

//----------------------------------Includes-------------------------------
#include <winsock2.h> 
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>



//------------------------------Function Declarations----------------------
int onMenu();
float avgCalculate(unsigned long* arr);
sockaddr_in getServer();
void chooseAndConcatCountry(char sendBuff [] );
using namespace std;

void main()
{
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "UDP Client: Error at WSAStartup()\n";
	}

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "UDP Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	sockaddr_in server = getServer();

	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	int choice = -1;

	while (choice != 13) {
		float average = 0;
		bool startDelayMeasure = false;
		bool startRTTMeasure = false;
		bool exitNow = false;
		choice = onMenu();
		while (choice <= 0 || choice > 13)
		{
			cout << "Error, please choose again" << endl;
			choice = onMenu();
		}

		switch (choice)
		{
			case 1:strcpy(sendBuff, "GetTime"); break;
			case 2:strcpy(sendBuff, "GetTimeWithoutDate"); break;
			case 3:strcpy(sendBuff, "GetTimeSinceEpoch"); break;
			case 4:
			{
				strcpy(sendBuff, "GetClientToServerDelayEstimation");
				startDelayMeasure = true;
				break;
			}
			case 5:
			{
				strcpy(sendBuff, "MeasureRTT");
				startRTTMeasure = true;
				break;
			}
			case 6:strcpy(sendBuff, "GetTimeWithoutDateOrSeconds"); break;
			case 7:strcpy(sendBuff, "GetYear"); break;
			case 8:strcpy(sendBuff, "GetMonthAndDay"); break;
			case 9:strcpy(sendBuff, "GetSecondsSinceBeginingOfMonth"); break;
			case 10:strcpy(sendBuff, "GetDayOfYear"); break;
			case 11:strcpy(sendBuff, "GetDaylightSavings"); break;
			case 12:
			{
				strcpy(sendBuff, "GetTimeWithoutDateInCity");
				chooseAndConcatCountry(sendBuff);
				break;
			}
			case 13: exitNow = true; break;
		}

		if (startRTTMeasure)
		{
			int i = 0;
			float avrg = 0;
			unsigned long responseArr[REQ_NUM];
			unsigned long  timeSentArr[REQ_NUM];

			strcpy(sendBuff, "GetTime");
			for (i = 0; i < REQ_NUM; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
				timeSentArr[i] = GetTickCount();

				if (SOCKET_ERROR == bytesSent)
				{
					cout << "UDP Client: Error at sendto(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}

				// Gets the server's answer using simple recieve (no need to hold the server's address).
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				responseArr[i] = GetTickCount() - timeSentArr[i];

				if (SOCKET_ERROR == bytesRecv)
				{
					cout << "UDP Client: Error at recv(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
				recvBuff[bytesRecv] = '\0';
			}//end for 

			avrg = avgCalculate(responseArr);
			cout << "The average Round Trip Round (RTT) is:  " << avrg << " miliseconds \n";
		}

		if (startDelayMeasure)
		{
			int i, j;
			float avrg = 0;
			unsigned long responseArr[REQ_NUM];
			unsigned long  timeSentArr[REQ_NUM];
			strcpy(sendBuff, "GetTime");
			for (i = 0; i < REQ_NUM; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
				timeSentArr[i] = GetTickCount();

				if (SOCKET_ERROR == bytesSent)
				{
					cout << "UDPClient: Error at sendto(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
			}

			for (i = 0; i < REQ_NUM; i++)
			{
				// Gets the server's answer
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				responseArr[i] = GetTickCount() - timeSentArr[i];

				if (SOCKET_ERROR == bytesSent)
				{
					cout << "UDP Client: Error at recv(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
				recvBuff[bytesRecv] = '\0';
			}
			avrg = avgCalculate(responseArr);
			cout << "The average delay between client and server is:  " << avrg << " miliseconds \n";
		}

		//for options numbers: 1-3 and 6-11 
		if (!startDelayMeasure && !startRTTMeasure && !exitNow) {
			bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
			if (SOCKET_ERROR == bytesSent)
			{
				cout << "UDP Client: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}
			cout << "UDP Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

			bytesRecv = recv(connSocket, recvBuff, 255, 0);
			if (SOCKET_ERROR == bytesRecv)
			{
				cout << "UDP Client: Error at recv(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}

			recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
			cout << "UDP Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
		}
	}

	// Closing connections and Winsock.
	cout << "UDP Client: Closing Connection.\n";
	closesocket(connSocket);

}

int onMenu() {
	int num = 0;

	cout << "Choose your action :" << endl;
	cout << "1) - Get time" << endl;
	cout << "2) - Get time without date" << endl;
	cout << "3) - Get time since 1.1.1970" << endl;
	cout << "4) - Get delay time " << endl;
	cout << "5) - Measure RTT - round trip time" << endl;
	cout << "6) - Get time without date or seconds" << endl;
	cout << "7) - Get year" << endl;
	cout << "8) - Get month and day" << endl;
	cout << "9) - Get seconds since begining of the month" << endl;
	cout << "10) - Get day of year" << endl;
	cout << "11) - Get daylight savings" << endl;
	cout << "12) - Get Time Without Date In City" << endl;
	cout << "13) - Exit" << endl;

	cin >> num;
	return num;
}

float avgCalculate(unsigned long* arr) {
	int sum = 0, i;
	for (i = 0; i < REQ_NUM; i++)
		sum += arr[i];
	return sum / REQ_NUM;
}

sockaddr_in getServer()
{
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);
	return server;
}

void chooseAndConcatCountry(char sendBuff[])
{
	char str[100];
	cout << "Enter City (Suporrted Cites: beijing, sydney, washington, rome):" << endl;
	cin.clear();
	cin.ignore(10000, '\n');
	cin.get(str, 100);
	strcat(sendBuff, str);
}
