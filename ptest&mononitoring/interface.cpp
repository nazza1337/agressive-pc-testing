#include "interface.h"
#include "load_workers.h"
#include <iostream>
#include <windows.h>

void DisplayStatus(const TemperatureMonitor& tempMonitor) {
    system("cls");
    std::cout << "=== Контроллер нагрузки CPU/GPU ===" << std::endl;
    std::cout << "Текущая нагрузка CPU: " << cpuLoadPercentage << "%" << std::endl;
    std::cout << "Текущая нагрузка GPU: " << gpuLoadPercentage << "%" << std::endl;

    float temp = tempMonitor.GetCPUTemperature();
    if (temp >= 0) {
        std::cout << "Температура CPU: " << temp << " °C" << std::endl;
    }
    else {
        std::cout << "Температура CPU: N/A (Запустите Open Hardware Monitor)" << std::endl;
    }

    std::cout << "\nУправление CPU:" << std::endl;
    std::cout << "1 - 25%\t2 - 50%\t3 - 75%\t4 - 100%\t0 - 0%" << std::endl;
    std::cout << "\nУправление GPU:" << std::endl;
    std::cout << "F1 - 0%\tF2 - 100%\t" << std::endl;
    std::cout << "\nESC - Выход" << std::endl;
}

void MonitorThread(const TemperatureMonitor& tempMonitor) {
    while (running) {
        DisplayStatus(tempMonitor);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}