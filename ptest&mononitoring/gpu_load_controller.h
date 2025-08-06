#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <atomic>

class GPULoadController {
public:
    GPULoadController();
    bool IsInitialized() const;
    void SetLoad(int percentage);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
    bool initialized = false;

    bool Initialize();
};