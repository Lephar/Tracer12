#include "pch.h"

#include "infrastructure.h"

#include "debug.h"

namespace tracer::graphics::infrastructure {
	namespace {
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = nullptr;
	}

	void initialize() {
		debug::print("Initializing infrastructure:");
		debug::incrementDepth();

		debug::verify::com(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf())));
		debug::print("DXGI factory created with debug mode enabled");

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
