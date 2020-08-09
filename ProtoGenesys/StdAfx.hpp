//=====================================================================================

#pragma once

#include <Windows.h>
#include <WinInet.h>
#include <Psapi.h>
#include <future>

#include <d3d11.h>
#include <detours.h>
#include <steam_api_common.h>
#include <isteamuser.h>
#include <isteamfriends.h>
#include <VMProtectSDK.h>

#include "ImGui/Addons/FileSystem/imguifilesystem.h"
#include "ImGui/Addons/TabWindow/imguitabwindow.h"
#include "ImGui/Addons/WICTextureLoader.h"
#include "ImGui/DX11/imgui_impl_dx11.h"
#include "ImGui/Fonts/Bank_Gothic_Pro_Light.hpp"
#include "ImGui/Fonts/Eurostile_Extended.hpp"
#include "ImGui/Win32/imgui_impl_win32.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_memory_editor.h"

#include "Core/Aimbot/Aimbot.h"
#include "Core/AntiAim/AntiAim.h"
#include "Core/Autowall/Autowall.h"
#include "Core/Console/Console.h"
#include "Core/Drawing/Drawing.h"
#include "Core/FurtiveCrash/FurtiveCrash.h"
#include "Core/FurtiveHook/FurtiveHook.h"
#include "Core/License/License.h"
#include "Core/Mathematics/Mathematics.h"
#include "Core/Profiler/Profiler.h"
#include "Core/Removals/Removals.h"
#include "Core/Stats/Stats.h"
#include "Core/TargetList/TargetList.h"
#include "Core/VMTHook/VMTHook.h"
#include "Core/Hooks.h"
#include "Core/MainGUI.h"

//=====================================================================================