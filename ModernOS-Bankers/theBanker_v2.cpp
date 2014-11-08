#include <Windows.h>
#include <fstream>
#include <iostream>

#define tellerNum 3	/* Assumed N = 3 */
#define inputFile "in.dat"

#define MaxCustomerNum 1000
#define MaxTellerNum 100
#define MaxTime 50
#define working TRUE
#define resting FALSE

int cusArg[MaxCustomerNum][3];
int cusnum;
HANDLE *cus;
HANDLE printlock;
HANDLE **servicemutex;
HANDLE teltime[tellerNum];
HANDLE *custime;
bool telstate[tellerNum];

class time
{
public:
	time(int t = 0);
	void timeplus();
	void cIn();
	int t;
}t;

time::time(int t)
{
	this->t = t;
}

void wait() //	Wait for all other process running.
{
	for (int i = 0; i < tellerNum + cusnum; i++)
	{
		Sleep(0);
	}
	return;
}

void time::timeplus()
{
	WaitForMultipleObjects(tellerNum, teltime, TRUE, INFINITE);
	WaitForMultipleObjects(cusnum, custime, TRUE, INFINITE);
	for (int i = 0; i < tellerNum; i++)
	{
		if (telstate[i] == working)
		{
			ResetEvent(teltime[i]);
		}
	}
	wait();
	this->t++;
}

DWORD WINAPI customer(LPVOID lpParam)
{
	int telNo;
	int *Param;
	Param = (int*)lpParam;
	ReleaseSemaphore(cus[Param[0] - 1], 1, NULL);
	Sleep(0);
	SetEvent(custime[Param[0] - 1]);
	/* May add time there, and can't right time that finish service */
	telNo = WaitForMultipleObjects(tellerNum, servicemutex[Param[0] - 1], false, INFINITE) - WAIT_OBJECT_0 + 1;
	ResetEvent(custime[Param[0] - 1]);
	WaitForSingleObject(printlock, INFINITE);
	std::cout << "CNo. " << Param[0] << " Enter: " << Param[1] << " Serve: " <<
		t.t - Param[2] << " Exit: " << t.t << " TNo. " << telNo << std::endl;
	ReleaseMutex(printlock);
	SetEvent(custime[Param[0] - 1]);
	return 0;
}

DWORD WINAPI teller(LPVOID lpParam)
{
	int *Param;
	int cusNo;
	Param = (int*)lpParam;
	SetEvent(teltime[Param[0]]);
	while (t.t < MaxTime)
	{
		telstate[Param[0]] = resting;
		SetEvent(teltime[Param[0]]);
		/* May add time there, and can't give service instantly */
		cusNo = WaitForMultipleObjects(cusnum, cus, false, INFINITE) - WAIT_OBJECT_0;
		ResetEvent(teltime[Param[0]]);
		telstate[Param[0]] = working;
		int exit = t.t + cusArg[cusNo][2];
		while (t.t < exit)
		{
			SetEvent(teltime[Param[0]]);
		}
		ResetEvent(teltime[Param[0]]);
		ReleaseSemaphore(servicemutex[cusNo][Param[0]], 1, NULL);
		Sleep(0);
	}
	return 0;
}

int main()
{
	// Init
	printlock = CreateMutex(NULL, FALSE, NULL);
	cusnum = 0;
	std::fstream input;
	input.open(inputFile);
	for (int i = 0; i < MaxCustomerNum; i++)
	{
		for (int j = 0; j < 3; j++)
			input >> cusArg[i][j];
		if (input.fail())
		{
			cusnum = i;
			break;
		}
	}
	input.close();
	cus = new(HANDLE[cusnum]);
	servicemutex = new(HANDLE*[cusnum]);
	custime = new(HANDLE[cusnum]);
	for (int i = 0; i < cusnum; i++)
	{
		cus[i] = CreateSemaphore(NULL, 0, 1, NULL);
		servicemutex[i] = new(HANDLE[tellerNum]);
		custime[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
		for (int j = 0; j < tellerNum; j++)
		{
			servicemutex[i][j] = CreateSemaphore(NULL, 0, 1, NULL);
		}
	}
	HANDLE telThreadId[tellerNum];
	HANDLE *cusThreadId;
	cusThreadId = new(HANDLE[cusnum]);
	for (int i = 0; i < tellerNum; i++)
	{
		int *telArg;
		telArg = new(int);
		telArg[0] = i;
		telThreadId[i] = CreateThread(NULL, 0, teller, telArg, 0, NULL);
		teltime[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	while (t.t < MaxTime)
	{
		for (int i = 0; i < cusnum; i++)
		{
			if (cusArg[i][1] == t.t)
			{
				cusThreadId[i] = CreateThread(NULL, 0, customer, cusArg[i], 0, NULL);
				ResetEvent(custime[i]);
			}
		}
		t.timeplus();
	}
	delete(cus);
	delete(cusThreadId);
	return 0;
}