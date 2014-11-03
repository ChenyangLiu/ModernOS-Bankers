#include <Windows.h>
#include <fstream>
#include <iostream>

#define tellerNum 3	/* Assumed N = 3 */
#define inputFile "in.dat"

#define MaxCustomerNum 1000
#define MaxTellerNum 100
#define MaxTime 50
#define resting TRUE
#define working FALSE

int time;
HANDLE service;
HANDLE tellerlock;
HANDLE printlock;
bool teller[tellerNum];

DWORD WINAPI customer(LPVOID lpParam)
{
	int *data;
	data = (int*)lpParam;
	// Not Enter
	while (time < data[1]);
	// Enter and wait
	int serviceTellerNum, getServiceTime, finishServiceTime;
	WaitForSingleObject(service, INFINITE);
	// Wait for the permission to change teller.
	WaitForSingleObject(tellerlock, INFINITE);
	for (int i = 0; i < tellerNum; i++)
	{
		if (teller[i] == resting)
		{
			teller[i] = working;
			serviceTellerNum = i;
			break;
		}
	}
	getServiceTime = time;
	finishServiceTime = getServiceTime + data[2];
	ReleaseMutex(tellerlock);
	while (time < finishServiceTime);
	WaitForSingleObject(printlock, INFINITE);
	std::cout << "CNo. " << data[0] << " Enter: " << data[1] << " Serve: " <<
		getServiceTime << " Exit: " << finishServiceTime << " TNo. " << serviceTellerNum + 1 << std::endl;
	teller[serviceTellerNum] = resting;
	ReleaseMutex(printlock);
	ReleaseSemaphore(service, 1, NULL);
	return 0;
}

int main()
{
	// Initialize.
	time = 0;
	service = CreateSemaphore(NULL, tellerNum, MaxTellerNum, NULL);
	tellerlock = CreateMutex(NULL, FALSE, NULL);
	printlock = CreateMutex(NULL, FALSE, NULL);
	for (int i = 0; i < tellerNum; i++)
	{
		teller[i] = resting;
	}
	// Create customer threads
	std::fstream input;
	input.open(inputFile);
	HANDLE cusThreadId[MaxCustomerNum];
	while (TRUE)
	{
		int *cusArg;
		cusArg = new int[3];
		for (int i = 0; i < 3; i++)
			input >> cusArg[i];
		if (input.fail())
			break;
		cusThreadId[cusArg[0] - 1] = CreateThread(NULL, 0, customer, cusArg, 0, NULL);
	}
	// Time is lasping.
	while (time < MaxTime)
	{
		Sleep(100);
		time++;
	}
	WaitForMultipleObjects(MaxCustomerNum, cusThreadId, TRUE, NULL);
	system("pause");
	return 0;
}