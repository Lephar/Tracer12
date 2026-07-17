#include "pch.h"

#include "system.h"

#include "debug.h"

namespace tracer::system {
	namespace {
		const char* title = nullptr;

		uint32_t width = 0;
		uint32_t height = 0;

		HINSTANCE instance = nullptr;
		HWND window = nullptr;

		std::filesystem::path dataFolder = {};

		DirectX::Mouse mouse = {};
		DirectX::Keyboard keyboard = {};
		DirectX::Mouse::State mouseState = {};
		DirectX::Keyboard::State keyboardState = {};
		DirectX::Mouse::ButtonStateTracker mouseStateTracker = {};
		DirectX::Keyboard::KeyboardStateTracker keyboardStateTracker = {};

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			if (uMsg == WM_DESTROY) {
				PostQuitMessage(EXIT_SUCCESS);
				return EXIT_SUCCESS;
			}
			else if (uMsg == WM_ACTIVATEAPP) {
				DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);
				DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
			}
			else if (uMsg == WM_MOUSEACTIVATE) {
				return MA_ACTIVATEANDEAT;
			}
			else if (uMsg == WM_KEYDOWN) {
				if (wParam == VK_ESCAPE) {
					DestroyWindow(hWnd);
				}
				else {
					DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
				}
			}
			else if (uMsg == WM_SYSKEYDOWN
				|| uMsg == WM_SYSKEYUP
				|| uMsg == WM_KEYUP
			) {
				DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
			}
			else if (uMsg == WM_ACTIVATE
				|| uMsg == WM_INPUT
				|| uMsg == WM_MOUSEMOVE
				|| uMsg == WM_LBUTTONDOWN
				|| uMsg == WM_LBUTTONUP
				|| uMsg == WM_RBUTTONDOWN
				|| uMsg == WM_RBUTTONUP
				|| uMsg == WM_MBUTTONDOWN
				|| uMsg == WM_MBUTTONUP
				|| uMsg == WM_MOUSEWHEEL
				|| uMsg == WM_XBUTTONDOWN
				|| uMsg == WM_XBUTTONUP
				|| uMsg == WM_MOUSEHOVER
			) {
				DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	void initialize(const char* windowTitle) {
		debug::print("Initializing system:");
		debug::incrementDepth();

		width = 800;
		height = 600;
		debug::print("Window size set");

		title = windowTitle;
		debug::print("Window title set");

		debug::verify::com(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
		debug::print("COM library initialized with multithread support");

		debug::verify::win(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, nullptr, &instance));
		debug::print("Instance handle acquired");

		WNDCLASSEX windowClass = {
			.cbSize = sizeof(WNDCLASSEX),
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = WndProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = instance,
			.hIcon = LoadIcon(nullptr, IDI_APPLICATION),
			.hCursor = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW),
			.lpszMenuName = nullptr,
			.lpszClassName = title,
			.hIconSm = LoadIcon(nullptr, IDI_APPLICATION)
		};

		debug::verify::win(RegisterClassEx(&windowClass));
		debug::print("Window class registered");

		window = CreateWindowEx(0, title, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);
		debug::verify::win(window);
		debug::print("Window created");

		debug::verify::negative(ShowWindow(window, SW_SHOWDEFAULT));
		debug::print("Window shown");

		debug::verify::positive(UpdateWindow(window));
		debug::print("Window updated");

		RECT rect;
		debug::verify::win(GetClientRect(window, &rect));
		width = rect.right;
		height = rect.bottom;
		debug::print("Window size: %ux%u", width, height);

		mouse.SetWindow(window);
		debug::print("Mouse window set");

		mouse.SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
		debug::print("Mouse mode set to relative");

		dataFolder = std::filesystem::current_path() / "data";
		debug::print("Data folder set: %s", dataFolder.string().c_str());

		debug::decrementDepth();
	}

	uint32_t getWidth() {
		return width;
	}

	uint32_t getHeight() {
		return height;
	}

	HWND getWindow() {
		return window;
	}

	std::filesystem::path getDataFolder() {
		return dataFolder;
	}

	bool poll() {
		MSG message;

		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT) {
				return false;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return true;
	}
	/*
	namespace {
		std::chrono::time_point<std::chrono::high_resolution_clock> timeCurrent = {};
		std::chrono::duration<float> timeDelta = {};

	}

	void aaaaaaaaaaaaaaaaaaa() {
		mouseStateTracker.Reset();
		keyboardStateTracker.Reset();
		debug::print("Mouse and keyboard state reset");

		timeCurrent = std::chrono::high_resolution_clock::now();
		debug::print("Current time saved");
	}

	bool poll() {
		MSG message;
		
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT) {
				return false;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		auto timePrevious = timeCurrent;
		timeCurrent = std::chrono::high_resolution_clock::now();
		timeDelta = timeCurrent - timePrevious;

		mouseState = mouse.GetState();
		keyboardState = keyboard.GetState();

		mouseStateTracker.Update(mouseState);
		keyboardStateTracker.Update(keyboardState);

		return true;
	}

	float getTimeDelta() {
		return timeDelta.count();
	}

	DirectX::Mouse::State getMouseState() {
		return mouseState;
	}

	DirectX::Keyboard::State getKeyboardState() {
		return keyboardState;
	}
	*/
}
