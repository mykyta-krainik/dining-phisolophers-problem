#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <conio.h>

std::vector<HANDLE> hPhilosThreads;
std::vector<HANDLE> hForks;
HANDLE printWait;
int philosophersForksAmount;

HANDLE getRightFork(int philNum) {
    return hForks[(philNum + 1) % philosophersForksAmount];
}

HANDLE getLeftFork(int philNum) {
    return hForks[philNum];
} 

void printInfPhilosopher(int philId) {
    WaitForSingleObject(printWait, INFINITE);

    ResetEvent(printWait);

    std::cout << "Philosopher " << philId << " is eating\n";

    SetEvent(printWait);

    Sleep(rand() % 3001 + 1000);

    WaitForSingleObject(printWait, INFINITE);

    ResetEvent(printWait);

    std::cout << "Philosopher " << philId << " has already finished eating\n";

    SetEvent(printWait);

}

DWORD WINAPI philosopher(LPVOID id) {
    int philId = *static_cast<int*>(id);
    DWORD lFork, rFork;

    while (true) {
        lFork = WaitForSingleObject(getLeftFork(philId), 0);

        if (lFork == WAIT_OBJECT_0) {
            ResetEvent(getLeftFork(philId));

            rFork = WaitForSingleObject(getRightFork(philId), 0);

            if (rFork == WAIT_OBJECT_0) {
                ResetEvent( getRightFork(philId) );

                printInfPhilosopher(philId);

                SetEvent( getRightFork(philId) );
            }

            SetEvent( getLeftFork(philId) );

            Sleep(2000);
        }
    }

    return 0;
}

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    std::cout << "Enter a number of philosophers and forks: ";
    std::cin >> philosophersForksAmount;

    hPhilosThreads.resize(philosophersForksAmount);
    hForks.resize(philosophersForksAmount);

    std::vector<DWORD> dwThreadParam(philosophersForksAmount);

    printWait = CreateEvent(NULL, true, true, NULL);

    for (int i = 0; i < philosophersForksAmount; i++) {
        hForks[i] = CreateEvent(NULL, true, true, NULL);

        if (!hForks[i]) {
            std::cout << "Erroe occurred" << std::endl;

            return -1;
        }
    }

    for (int i = 0; i < philosophersForksAmount; i++) {
        dwThreadParam[i] = i;
        hPhilosThreads[i] = CreateThread(NULL, 0, philosopher, &dwThreadParam[i], 0, NULL);

        if (!hPhilosThreads[i]) {
            std::cout << "Erroe occurred" << std::endl;

            return -1;
        }
    }

    _getch();

    for (int i = 0; i < philosophersForksAmount; i++) {
        CloseHandle(hPhilosThreads[i]);
    }


    for (int i = 0; i < philosophersForksAmount; i++) {
        CloseHandle(hForks[i]);
    }

    return 0;
}
