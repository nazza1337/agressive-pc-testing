#include "temperature_monitor.h"
#include <iostream>
#include <algorithm>
#define NOMINMAX
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "oleaut32.lib")

TemperatureMonitor::TemperatureMonitor() {
    InitializeCom();
}

TemperatureMonitor::~TemperatureMonitor() {
    if (m_comInitialized) {
        CoUninitialize();
    }
}

float TemperatureMonitor::GetCPUTemperature() const {
    if (!m_comInitialized) {
        return -1.0f;
    }

    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;
    IEnumWbemClassObject* pEnumerator = nullptr;
    float temperature = -1.0f;

    try {
        HRESULT hr = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            reinterpret_cast<LPVOID*>(&pLoc)
        );
        if (FAILED(hr)) throw _com_error(hr);

        hr = pLoc->ConnectServer(
            _bstr_t(L"ROOT\\OpenHardwareMonitor"),
            nullptr,
            nullptr,
            nullptr,
            0L,
            nullptr,
            nullptr,
            &pSvc
        );
        if (FAILED(hr)) throw _com_error(hr);

        hr = CoSetProxyBlanket(
            pSvc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            nullptr,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr,
            EOAC_NONE
        );
        if (FAILED(hr)) throw _com_error(hr);

        hr = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Sensor WHERE SensorType='Temperature' AND Name LIKE '%CPU%'"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr,
            &pEnumerator
        );
        if (FAILED(hr)) throw _com_error(hr);

        IWbemClassObject* pclsObj = nullptr;
        ULONG uReturn = 0;
        float maxTemp = -1.0f;

        while (pEnumerator && SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn))) {
            if (uReturn == 0) break;

            VARIANT vtName, vtValue;
            VariantInit(&vtName);
            VariantInit(&vtValue);

            if (SUCCEEDED(pclsObj->Get(L"Name", 0L, &vtName, nullptr, nullptr)) &&
                SUCCEEDED(pclsObj->Get(L"Value", 0L, &vtValue, nullptr, nullptr))) {
                std::wstring name(vtName.bstrVal);
                if (name.find(L"CPU") != std::wstring::npos ||
                    name.find(L"Core") != std::wstring::npos) {
                    temperature = vtValue.fltVal;
                    maxTemp = (std::max)(maxTemp, temperature);
                }
            }

            VariantClear(&vtName);
            VariantClear(&vtValue);
            pclsObj->Release();
        }

        if (maxTemp > 0) {
            temperature = maxTemp;
        }
    }
    catch (const _com_error& e) {
        LogError(e);
        temperature = -1.0f;
    }

    SafeRelease(pEnumerator);
    SafeRelease(pSvc);
    SafeRelease(pLoc);

    return temperature;
}

void TemperatureMonitor::InitializeCom() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "COM initialization failed: 0x" << std::hex << hr << std::endl;
        return;
    }

    hr = CoInitializeSecurity(
        nullptr,
        -1L,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr
    );

    if (FAILED(hr)) {
        std::cerr << "Security initialization failed: 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return;
    }

    m_comInitialized = true;
}

void TemperatureMonitor::LogError(const _com_error& e) const {
    std::cerr << "WMI Error: " << e.ErrorMessage() << " (0x"
        << std::hex << e.Error() << ")" << std::endl;
}

template<class T>
void TemperatureMonitor::SafeRelease(T*& p) const {
    if (p != nullptr) {
        p->Release();
        p = nullptr;
    }
}