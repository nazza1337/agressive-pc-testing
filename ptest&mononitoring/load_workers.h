#pragma once
#include <atomic>
#include "gpu_load_controller.h"
#include <thread>

extern std::atomic<int> cpuLoadPercentage;
extern std::atomic<int> gpuLoadPercentage;
extern std::atomic<bool> running;

void CPULoadWorker();
void GPULoadWorker(GPULoadController& gpuController);