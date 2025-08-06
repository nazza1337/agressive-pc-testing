#include "gpu_load_controller.h"
#include "temperature_monitor.h"
#include "load_workers.h"
#include "interface.h"
#include <iostream>
#include <thread>
#include <vector>
#include <conio.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "Russian");

    GPULoadController gpuController;
    if (!gpuController.IsInitialized()) {
        std::cout << "Предупреждение: Не удалось инициализировать GPU. Нагрузка GPU будет недоступна." << std::endl;
    }

    TemperatureMonitor tempMonitor;
    if (tempMonitor.GetCPUTemperature() < 0) {
        std::cout << "Предупреждение: Не удалось инициализировать мониторинг температуры." << std::endl;
        std::cout << "Установите и запустите Open Hardware Monitor для отображения температуры." << std::endl;
    }

    unsigned int numCores = std::thread::hardware_concurrency();
    if (numCores == 0) numCores = 1;

    std::vector<std::thread> cpuThreads;
    for (unsigned int i = 0; i < numCores; ++i) {
        cpuThreads.emplace_back(CPULoadWorker);
    }

    std::thread gpuThread(GPULoadWorker, std::ref(gpuController));
    std::thread monitorThread(MonitorThread, std::ref(tempMonitor));

    while (running) {
        if (_kbhit()) {
            int key = _getch();

            switch (key) {
            case '0': cpuLoadPercentage = 0; break;
            case '1': cpuLoadPercentage = 25; break;
            case '2': cpuLoadPercentage = 50; break;
            case '3': cpuLoadPercentage = 75; break;
            case '4': cpuLoadPercentage = 100; break;
            }

            if (key == 0) {
                key = _getch();
                switch (key) {
                case 59: gpuLoadPercentage = 0; break;
                case 60: gpuLoadPercentage = 100; break;
                }
            }

            if (key == 27) running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    for (auto& thread : cpuThreads) {
        thread.join();
    }

    gpuThread.join();
    monitorThread.join();

    std::cout << "Программа завершена" << std::endl;
    return 0;
}