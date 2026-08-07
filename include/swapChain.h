#pragma once

#include "pch.h"

namespace tracer::swapChain {
	void initialize();

	uint32_t getImageCount();
	uint32_t getSampleCount();

	DXGI_FORMAT getDepthStencilFormat();
	DXGI_FORMAT getRenderTargetFormat();

	void createResources();

	void begin();
	Microsoft::WRL::ComPtr<ID3D12Resource2> getCurrentConstantBuffer();
	void end();
	void present();

	void destroy();
}
