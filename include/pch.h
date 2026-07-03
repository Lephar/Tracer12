#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// STL
#include <print>
#include <vector>
#include <memory>
#include <chrono>
#include <numbers>
#include <fstream>
#include <optional>
#include <filesystem>

// VC++
#include <windows.h>
#include <comdef.h>
#include <wrl/client.h>
#include <wincodec.h>

// DirectX
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dx12.h>
#include <DescriptorHeap.h>
#include <CommonStates.h>
#include <SimpleMath.h>
#include <Mouse.h>
#include <Keyboard.h>

// 3rd Party
#include "cgltf/cgltf.h"
