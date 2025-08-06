#pragma once
#include <comdef.h>
#include <Wbemidl.h>
#include <string>

class TemperatureMonitor {
public:
    TemperatureMonitor();
    ~TemperatureMonitor();
    float GetCPUTemperature() const;

private:
    bool m_comInitialized = false;

    void InitializeCom();
    void LogError(const _com_error& e) const;
    template<class T> void SafeRelease(T*& p) const;
};