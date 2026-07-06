#pragma once

#include "pch.h"

namespace tracer::graphics {
	class Pipeline {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;

	public:
		Pipeline(LPCWSTR vertexShaderName, LPCWSTR pixelShaderName);

		Pipeline(const Pipeline& pipeline) = delete;
		Pipeline& operator=(const Pipeline& pipeline) = delete;

		Pipeline(Pipeline&& pipeline) = delete;
		Pipeline& operator=(Pipeline&& pipeline) = delete;

		void bind();

		~Pipeline();
	};
}
