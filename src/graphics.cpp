#include "pch.h"

#include "graphics.h"

#include "verify.h"

extern "C" {
	__declspec(dllexport) extern const uint32_t D3D12SDKVersion = 619;
	__declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

namespace tracer::graphics {
	namespace {
		Microsoft::WRL::ComPtr<IDxcCompiler3> compiler = nullptr;
		Microsoft::WRL::ComPtr<IDxcUtils> utils = nullptr;
		
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Debug6> debug = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device15> device = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debugDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12InfoQueue1> infoQueue = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DebugCommandQueue1> debugCommandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList = nullptr;

		DWORD callbackCookie = 0;

		void __stdcall CallbackFunc(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID ID, LPCSTR pDescription, void* pContext) {
			std::println("{}", pDescription);
		}
	}

	void initialize() {
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

		VERIFY_COM(DxcCreateInstance2(nullptr, CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
		std::println("Shader compiler created");

		VERIFY_COM(DxcCreateInstance2(nullptr, CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));
		std::println("Shader compilation utilities created");

		VERIFY_COM(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf())));
		std::println("DXGI factory created with debug mode enabled");

		VERIFY_COM(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf())));

		DXGI_ADAPTER_DESC3 adapterDesc;
		adapter->GetDesc3(&adapterDesc);
		char adapterDescription[UCHAR_MAX];
		wsprintf(adapterDescription, "%ws", adapterDesc.Description);

		std::println("High performance adapter {} selected", adapterDescription);

		VERIFY_COM(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(device.GetAddressOf())));
		std::println("Device created with feature set 12.2");

		VERIFY_COM(device->QueryInterface(IID_PPV_ARGS(debugDevice.GetAddressOf())));
		std::println("Debug device created from device");

		VERIFY_COM(device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())));
		std::println("Info queue created from device");

		VERIFY_COM(infoQueue->RegisterMessageCallback(CallbackFunc, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &callbackCookie));
		std::println("Debug message callback registered");

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 1,
		};

		VERIFY_COM(device->CreateCommandQueue1(&commandQueueDesc, {}, IID_PPV_ARGS(commandQueue.GetAddressOf())));
		std::println("Direct command queue created");

		VERIFY_COM(commandQueue->QueryInterface(IID_PPV_ARGS(debugCommandQueue.GetAddressOf())));
		std::println("Debug command queue created from command queue");

		VERIFY_COM(device->CreateCommandList1(1, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(commandList.GetAddressOf())));
		std::println("Direct command list created");
	}

	Microsoft::WRL::ComPtr<IDxcCompiler3> getCompiler() {
		return compiler;
	}

	Microsoft::WRL::ComPtr<IDxcUtils> getCompilerUtils() {
		return utils;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory7> getFactory() {
		return factory;
	}

	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice() {
		return device;
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue1> getCommandQueue() {
		return commandQueue;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList() {
		return commandList;
	}
}
