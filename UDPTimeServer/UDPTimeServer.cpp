//----------------------------------Defines--------------------------------
#define _CRT_SECURE_NO_WARNINGS
#define TIME_PORT	27015
#define ROME_DELTA (+2)	
#define WASHINGTOM_DELTA (-4)
#define BEIJING_DELTA (+8)
#define SYDNEY_DELTA (+10)

//----------------------------------Includes-------------------------------
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <chrono>

//----------------------------------Globals--------------------------------
char sendBuff[255];

//------------------------------Function Declarations----------------------
void handleGetTime();
sockaddr_in getServerService();
void handleGetTimeWithoutDateOrSeconds();
void handleGetYear();
void handleGetMonthAndDay();
void handleGetSecondsSinceBeginingOfMonth();
void handleGetDayOfYear();
void handleGetDaylightSavings();
void handleGetTimeWithoutDateInCity(char* country);
void handleTask(char recvBuff[]);
void handleGetTimeSinceEpoch();
void handleGetTimeWithoutDate();

using namespace std;

void main()
{
	WSAData wsaData;

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Server: Error at WSAStartup()\n";
		return;
	}

	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_socket)
	{
		cout << "Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	sockaddr_in serverService = getServerService();

	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char recvBuff[255];

	cout << "Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		handleTask(recvBuff);

		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

tm* getLocalTimeinfo()
{
	time_t rawtime;
	tm* localTimeInfo;
	time(&rawtime);
	localTimeInfo = localtime(&rawtime);
	return localTimeInfo;
}
void handleGetTimeWithoutDateOrSeconds()
{
	tm* localTimeinfo = getLocalTimeinfo();

	strftime(sendBuff, 255, "%H:%M", localTimeinfo);
	puts(sendBuff);
}

void handleGetYear()
{
	tm* localTimeinfo = getLocalTimeinfo();

	strftime(sendBuff, 255, "%y", localTimeinfo);
	puts(sendBuff);
}

void handleGetMonthAndDay()
{
	tm* localTimeinfo = getLocalTimeinfo();

	strftime(sendBuff, 255, "%d.%m", localTimeinfo);
	puts(sendBuff);
}

void handleGetSecondsSinceBeginingOfMonth()
{
	tm* localTimeinfo = getLocalTimeinfo();

	int secondSinceToday = localTimeinfo->tm_sec + localTimeinfo->tm_min * 60 + localTimeinfo->tm_hour * 3600;
	int secondSinceBeginingMonth = 24 * (localTimeinfo->tm_mday - 1) * 3600; +secondSinceToday;
	_itoa(secondSinceBeginingMonth, sendBuff, 10);
}

void handleGetDayOfYear()
{
	tm* localTimeinfo = getLocalTimeinfo();

	int dayNumber = localTimeinfo->tm_yday;
	_itoa(dayNumber, sendBuff, 10);
}

void handleGetDaylightSavings()
{
	int* __cdecl dayLight = __daylight();
	_itoa(*dayLight, sendBuff, 10);
}

void handleGetTimeWithoutDateInCity(char* country)
{
	char message[] = "Country not supoorted! UTC time is  ";
	time_t rawtime;
	tm* globalTimeInfo;

	time(&rawtime);
	globalTimeInfo = gmtime(&rawtime);

	bool isSupportedCountry;
	if (isSupportedCountry = (strcmp(country, "beijing") == 0))
	{
		globalTimeInfo->tm_hour = (globalTimeInfo->tm_hour + BEIJING_DELTA) % 24;
	}
	else if (isSupportedCountry = (strcmp(country, "sydney") == 0))
	{
		globalTimeInfo->tm_hour = (globalTimeInfo->tm_hour + SYDNEY_DELTA) % 24;
	}
	else if (isSupportedCountry = (strcmp(country, "washington") == 0))
	{
		globalTimeInfo->tm_hour = (globalTimeInfo->tm_hour + WASHINGTOM_DELTA + 24) % 24;
	}
	else if (isSupportedCountry = (strcmp(country, "rome") == 0))
	{
		globalTimeInfo->tm_hour = (globalTimeInfo->tm_hour + ROME_DELTA) % 24;
	}

	if (!isSupportedCountry) {
		strcpy(sendBuff, message);
		strftime(sendBuff + strlen(message) - 1, 255, "%H:%M:%S", globalTimeInfo);
	}
	else
	{
		strftime(sendBuff, 255, "%H:%M:%S", globalTimeInfo);
	}
}

void handleTask(char recvBuff[])
{
	if (strcmp(recvBuff, "GetTime") == 0)
	{
		handleGetTime();
	}
	else if (strcmp(recvBuff, "GetTimeWithoutDate") == 0)
	{
		handleGetTimeWithoutDate();
	}
	else if (strcmp(recvBuff, "GetTimeSinceEpoch") == 0)
	{
		handleGetTimeSinceEpoch();
	}
	else 	if (strcmp(recvBuff, "GetYear") == 0)
	{
		handleGetYear();
	}
	else 	if (strcmp(recvBuff, "GetTimeWithoutDateOrSeconds") == 0)
	{
		handleGetTimeWithoutDateOrSeconds();
	}
	else 	if (strcmp(recvBuff, "GetMonthAndDay") == 0)
	{
		handleGetMonthAndDay();
	}
	else 	if (strcmp(recvBuff, "GetSecondsSinceBeginingOfMonth") == 0)
	{
		handleGetSecondsSinceBeginingOfMonth();
	}
	else 	if (strcmp(recvBuff, "GetDayOfYear") == 0)
	{
		handleGetDayOfYear();
	}
	else 	if (strcmp(recvBuff, "GetDaylightSavings") == 0)
	{
		handleGetDaylightSavings();
	}
	else 	if (strstr(recvBuff, "GetTimeWithoutDateInCity") == recvBuff)
	{
		handleGetTimeWithoutDateInCity(recvBuff + strlen("GetTimeWithoutDateInCity"));				// protocol
	}
}

void handleGetTimeSinceEpoch()
{
	time_t seconds;

	seconds = time(NULL);
	_itoa(seconds, sendBuff, 10);
}

void handleGetTime()
{
	time_t timer;
	time(&timer);
	strcpy(sendBuff, ctime(&timer));
	sendBuff[strlen(sendBuff) - 1] = '\0';
}

sockaddr_in getServerService()
{
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(TIME_PORT);
	return serverService;
}

void handleGetTimeWithoutDate()
{
	tm* localTimeinfo = getLocalTimeinfo();
	strftime(sendBuff, 255, "%H:%M:%S", localTimeinfo);
	puts(sendBuff);
}
