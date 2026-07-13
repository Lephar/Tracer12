#include "pch.h"

#include "device.h"

#include "debug.h"

namespace tracer::graphics::device {
	namespace {
		Microsoft::WRL::ComPtr<ID3D12Debug6> debug = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device15> device = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debugDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12InfoQueue1> infoQueue = nullptr;

		DWORD callbackCookie = 0;

		void __stdcall CallbackFunc(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID ID, LPCSTR pDescription, void* pContext) {
			debug::print("%s", pDescription);
		}
	}

	void initialize(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter) {
		debug::print("Initializing device:");
		debug::incrementDepth();

		debug::verify::com(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())));
		debug::print("Debug controller acquired");

		debug->EnableDebugLayer();
		debug::print("Debug layers enabled");

		debug->SetEnableGPUBasedValidation(true);
		debug::print("GPU based validations enabled");

		debug->SetEnableSynchronizedCommandQueueValidation(true);
		debug::print("Synchronized command queue validations enabled");

		debug->SetEnableAutoName(true);
		debug::print("Device objects auto naming enabled");

		debug::verify::com(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(device.GetAddressOf())));
		debug::print("Device created with feature level 12.2");

		debug::verify::com(device->QueryInterface(IID_PPV_ARGS(debugDevice.GetAddressOf())));
		debug::print("Debug device created from device");

		debug::verify::com(device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())));
		debug::print("Info queue created from device");

		debug::verify::com(infoQueue->RegisterMessageCallback(CallbackFunc, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &callbackCookie));
		debug::print("Debug message callback registered");

		debug::decrementDepth();
	}

	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice() {
		return device;
	}
}
