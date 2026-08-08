#include "pch.h"
#include "infrastructure.h"
#include "debug.h"

extern "C" {
	__declspec(dllexport) extern const uint32_t D3D12SDKVersion = 619;
	__declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

namespace tracer::infrastructure {
	namespace {
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = nullptr;
	}

	void initialize() {
		debug::print("Initializing infrastructure:");
		debug::incrementDepth();

		debug::verify::com(CreateDXGIFactory2(debug::enabled ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(factory.GetAddressOf())));
		debug::print("DXGI factory created %s", debug::enabled ? "with debug mode enabled" : "");

		debug::verify::com(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf())));

		DXGI_ADAPTER_DESC3 adapterDesc;
		adapter->GetDesc3(&adapterDesc);
		debug::print("High performance adapter %S selected", adapterDesc.Description);

		debug::decrementDepth();
	}

	Microsoft::WRL::ComPtr<IDXGIFactory7> getFactory() {
		return factory;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> getAdapter() {
		return adapter;
	}
}
