#include "pch.h"

#include "infrastructure.h"

#include "verify.h"

namespace tracer::graphics::infrastructure {
	namespace {
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = nullptr;
	}

	void initialize() {
		VERIFY_COM(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf())));
		std::println("DXGI factory created with debug mode enabled");

		VERIFY_COM(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf())));

		DXGI_ADAPTER_DESC3 adapterDesc;
		adapter->GetDesc3(&adapterDesc);
		char adapterDescription[UCHAR_MAX];
		wsprintf(adapterDescription, "%ws", adapterDesc.Description);

		std::println("High performance adapter {} selected", adapterDescription);
	}

	Microsoft::WRL::ComPtr<IDXGIFactory7> getFactory() {
		return factory;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> getAdapter() {
		return adapter;
	}
}
