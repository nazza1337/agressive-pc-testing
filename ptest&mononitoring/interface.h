#pragma once
#include "temperature_monitor.h"

void DisplayStatus(const TemperatureMonitor& tempMonitor);
void MonitorThread(const TemperatureMonitor& tempMonitor);