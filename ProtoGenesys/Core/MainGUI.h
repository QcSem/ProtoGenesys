//=====================================================================================

#pragma once

#include "Engine/Engine.hpp"

//=====================================================================================

#define MAX_VIRTUALKEYS 0x100

#define DEFAULT_CFG "\\" PROGRAM_NAME ".cfg"
#define DEFAULT_DMP "\\" PROGRAM_NAME ".dmp"
#define DEFAULT_INI "\\" PROGRAM_NAME ".ini"
#define DEFAULT_LOG "\\" PROGRAM_NAME ".log"
#define DEFAULT_TXT "\\" PROGRAM_NAME ".txt"
#define DEFAULT_XML "\\" PROGRAM_NAME ".xml"

//=====================================================================================

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=====================================================================================

namespace ProtoGenesys
{
	class cMainGUI
	{
	public:

		cMainGUI() : _thunkWindowProcess(&cMainGUI::WindowProcess, this) {}

		bool bInitialized, bStyleChanged, bShowWindow, bWriteLog, bIsAirStuck;

		std::string szIniFileName, szLogFileName;
		ImGuiFs::Dialog SaveDialog, LoadDialog;

		ImFont* Eurostile_Bold, * Eurostile_Extended, * Eurostile_Regular;
		float flEurostile_Bold, flEurostile_Extended, flEurostile_Regular;

		struct sVirtualKeys
		{
			bool bKey, bDown, bUp;
		} VirtualKeys[MAX_VIRTUALKEYS];

		HWND hWindow;
		HINSTANCE hInstDll;
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pDeviceContext;

		ID3D11Resource* pD3D11Resource;
		ID3D11ShaderResourceView* pD3D11ShaderResourceView;

		void InitInterface();
		void LoadBackgroundImage();
		void SetMenuColor(int index);
		void SetMenuCursor(int index);
		void SetMenuFont(int index);
		void RefreshInterface(int color, int cursor, int font);
		bool GetKeyPress(int vkey, bool immediate);

		void WINAPI Present(_In_ IDXGISwapChain* pSwapChain, _In_ UINT SyncInterval, _In_ UINT Flags);
		LRESULT WindowProcess(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		typedef LRESULT(CALLBACK* tWindowProcess)(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		tWindowProcess oWindowProcess;

		template<typename Type>
		bool DrawOption(std::string label, std::string option, Type* value, Type min, Type max, Type step);
		bool DrawColorPicker(std::string label, ImVec4& color);

		StdThunk<tWindowProcess, cMainGUI> _thunkWindowProcess;
	} extern _mainGui;
}

//=====================================================================================
