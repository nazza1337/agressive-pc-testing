#include "load_workers.h"
#include <chrono>
#include <cmath>
#include <thread>

std::atomic<int> cpuLoadPercentage(0);
std::atomic<int> gpuLoadPercentage(0);
std::atomic<bool> running(true);

void CPULoadWorker() {
    while (running) {
        int currentLoad = cpuLoadPercentage.load();

        if (currentLoad == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        const auto workTime = std::chrono::milliseconds(10);
        const auto totalCycleTime = std::chrono::milliseconds(10 * 100 / currentLoad);

        auto start = std::chrono::steady_clock::now();
        auto end = start + workTime;

        while (std::chrono::steady_clock::now() < end && running) {
            volatile double x = 0;
            for (int i = 0; i < 10000; ++i) {
                x += std::sin(i) * std::cos(i);
            }
        }

        auto sleepTime = totalCycleTime - workTime;
        if (sleepTime.count() > 0 && running) {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void GPULoadWorker(GPULoadController& gpuController) {
    while (running) {
        int currentLoad = gpuLoadPercentage.load();
        if (currentLoad > 0) {
            gpuController.SetLoad(currentLoad);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}