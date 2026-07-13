#pragma once

#include "pch.h"

namespace tracer::graphics::queue {
	void initialize(Microsoft::WRL::ComPtr<ID3D12Device15> device);

	Microsoft::WRL::ComPtr<ID3D12CommandQueue1> getCommandQueue();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList();

	void begin();
	void end();
	void execute();
	void signal();
	void wait();
}
