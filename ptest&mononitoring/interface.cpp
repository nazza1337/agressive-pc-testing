#include "interface.h"
#include "load_workers.h"
#include <iostream>
#include <windows.h>

void DisplayStatus(const TemperatureMonitor& tempMonitor) {
    system("cls");
    std::cout << "=== ���������� �������� CPU/GPU ===" << std::endl;
    std::cout << "������� �������� CPU: " << cpuLoadPercentage << "%" << std::endl;
    std::cout << "������� �������� GPU: " << gpuLoadPercentage << "%" << std::endl;

    float temp = tempMonitor.GetCPUTemperature();
    if (temp >= 0) {
        std::cout << "����������� CPU: " << temp << " �C" << std::endl;
    }
    else {
        std::cout << "����������� CPU: N/A (��������� Open Hardware Monitor)" << std::endl;
    }

    std::cout << "\n���������� CPU:" << std::endl;
    std::cout << "1 - 25%\t2 - 50%\t3 - 75%\t4 - 100%\t0 - 0%" << std::endl;
    std::cout << "\n���������� GPU:" << std::endl;
    std::cout << "F1 - 0%\tF2 - 100%\t" << std::endl;
    std::cout << "\nESC - �����" << std::endl;
}

void MonitorThread(const TemperatureMonitor& tempMonitor) {
    while (running) {
        DisplayStatus(tempMonitor);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}