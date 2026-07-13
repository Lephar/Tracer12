#include "pch.h"

#include "device.h"

#include "verify.h"

namespace tracer::graphics::device {
	namespace {
		Microsoft::WRL::ComPtr<ID3D12Debug6> debug = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device15> device = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debugDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12InfoQueue1> infoQueue = nullptr;

		DWORD callbackCookie = 0;

		void __stdcall CallbackFunc(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID ID, LPCSTR pDescription, void* pContext) {
			std::println("{}", pDescription);
		}
	}

	void initialize(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter) {
		VERIFY_COM(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())));
		std::println("Debug controller acquired");

		debug->EnableDebugLayer();
		std::println("Debug layers enabled");

		debug->SetEnableGPUBasedValidation(true);
		std::println("GPU based validations enabled");

		debug->SetEnableSynchronizedCommandQueueValidation(true);
		std::println("Synchronized command queue validations enabled");

		debug->SetEnableAutoName(true);
		std::println("Device objects auto naming enabled");

		VERIFY_COM(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(device.GetAddressOf())));
		std::println("Device created with feature set 12.2");

		VERIFY_COM(device->QueryInterface(IID_PPV_ARGS(debugDevice.GetAddressOf())));
		std::println("Debug device created from device");

		VERIFY_COM(device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())));
		std::println("Info queue created from device");

		VERIFY_COM(infoQueue->RegisterMessageCallback(CallbackFunc, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &callbackCookie));
		std::println("Debug message callback registered");
	}

	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice() {
		return device;
	}
}
