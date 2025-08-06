#include "gpu_load_controller.h"
#include <d3dcompiler.h>
#include <iostream>
#include "load_workers.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;

GPULoadController::GPULoadController() {
    Initialize();
}

bool GPULoadController::IsInitialized() const {
    return initialized;
}

void GPULoadController::SetLoad(int percentage) {
    if (!initialized || percentage < 0 || percentage > 100) return;

    const UINT dispatchCount = static_cast<UINT>(percentage * 50);
    const UINT iterations = 1000;

    struct Constants {
        UINT iterations;
        UINT padding[3];
    } constants = { iterations };

    ComPtr<ID3D11Buffer> constantBuffer;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(Constants);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&cbDesc, nullptr, &constantBuffer);

    context->CSSetShader(computeShader.Get(), nullptr, 0);
    context->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    for (int i = 0; i < 10 && running; i++) {
        D3D11_MAPPED_SUBRESOURCE mapped;
        context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, &constants, sizeof(Constants));
        context->Unmap(constantBuffer.Get(), 0);

        context->Dispatch(dispatchCount / 10, 1, 1);

        if (percentage < 100) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(10 * (100 - percentage) / percentage)
            );
        }
    }
}

bool GPULoadController::Initialize() {
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &context
    );

    if (FAILED(hr)) return false;

    const char* shaderCode =
        "cbuffer Constants { uint iterations; };\n"
        "[numthreads(64, 1, 1)]\n"
        "void CSMain(uint3 id : SV_DispatchThreadID) {\n"
        "    float result = 0.0f;\n"
        "    for (uint i = 0; i < iterations; i++) {\n"
        "        result += sin(float(i)) * cos(float(id.x));\n"
        "    }\n"
        "}";

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;
    hr = D3DCompile(
        shaderCode,
        strlen(shaderCode),
        nullptr,
        nullptr,
        nullptr,
        "CSMain",
        "cs_5_0",
        0,
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }

    hr = device->CreateComputeShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        &computeShader
    );

    initialized = SUCCEEDED(hr);
    return initialized;
}