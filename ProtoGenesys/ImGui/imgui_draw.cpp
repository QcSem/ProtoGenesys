// dear imgui, v1.76
// (drawing and font code)

/*

Index of this file:

// [SECTION] STB libraries implementation
// [SECTION] Style functions
// [SECTION] ImDrawList
// [SECTION] ImDrawListSplitter
// [SECTION] ImDrawData
// [SECTION] Helpers ShadeVertsXXX functions
// [SECTION] ImFontConfig
// [SECTION] ImFontAtlas
// [SECTION] ImFontAtlas glyph ranges helpers
// [SECTION] ImFontGlyphRangesBuilder
// [SECTION] ImFont
// [SECTION] ImGui Internal Render Helpers
// [SECTION] Decompression code
// [SECTION] Default font data (ProggyClean.ttf)

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca)
#if defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__) || defined(__APPLE__) || defined(__SWITCH__)
#include <alloca.h>     // alloca (glibc uses <alloca.h>. Note that Cygwin may have _WIN32 defined, so the order matters here)
#elif defined(_WIN32)
#include <malloc.h>     // alloca
#if !defined(alloca)
#define alloca _alloca  // for clang with MS Codegen
#endif
#else
#include <stdlib.h>     // alloca
#endif
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wcomma")
#pragma clang diagnostic ignored "-Wcomma"                  // warning : possible misuse of comma operator here             //
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"      // warning : macro name is a reserved identifier                //
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wstack-protector"          // warning: stack protector not protecting local variables: variable length buffer
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries implementation
//-------------------------------------------------------------------------

// Compile time options:
//#define IMGUI_STB_NAMESPACE           ImStb
//#define IMGUI_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define IMGUI_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456)                             // declaration of 'xx' hides previous local declaration
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wcast-qual"              // warning : cast from 'const xxxx *' to 'xxx *' drops const qualifier //
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"              // warning: comparison is always true due to limited range of data type [-Wtype-limits]
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'const xxxx *' to type 'xxxx *' casts away qualifiers
#endif

#ifndef STB_RECT_PACK_IMPLEMENTATION                        // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT(x)     do { IM_ASSERT(x); } while (0)
#define STBRP_SORT          ImQsort
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#ifdef IMGUI_STB_RECT_PACK_FILENAME
#include IMGUI_STB_RECT_PACK_FILENAME
#else
#include "STB/imstb_rectpack.h"
#endif
#endif

#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     do { IM_ASSERT(x); } while(0)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorStd(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef IMGUI_STB_TRUETYPE_FILENAME
#include IMGUI_STB_TRUETYPE_FILENAME
#else
#include "STB/imstb_truetype.h"
#endif
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// [SECTION] Style functions
//-----------------------------------------------------------------------------

#include "Addons/TabWindow/imguitabwindow.h"

void ImGui::StyleTabLabels(ImVec4* colors)
{
    TabLabelStyle& style = TabLabelStyle::Get();

    style.rounding = 0.0f;
    style.fillColorGradientDeltaIn0_05 = 0.0f;

    style.colors[TabLabelStyle::Col_TabLabel]                   = ColorConvertFloat4ToU32(colors[ImGuiCol_Header]);
    style.colors[TabLabelStyle::Col_TabLabelHovered]            = ColorConvertFloat4ToU32(colors[ImGuiCol_HeaderHovered]);
    style.colors[TabLabelStyle::Col_TabLabelActive]             = ColorConvertFloat4ToU32(colors[ImGuiCol_HeaderActive]);
    style.colors[TabLabelStyle::Col_TabLabelBorder]             = ColorConvertFloat4ToU32(colors[ImGuiCol_Header]);
    style.colors[TabLabelStyle::Col_TabLabelText]               = ColorConvertFloat4ToU32(ImVec4(0.00f, 0.00f, 0.00f, 1.00f));
    style.colors[TabLabelStyle::Col_TabLabelSelected]           = ColorConvertFloat4ToU32(colors[ImGuiCol_Button]);
    style.colors[TabLabelStyle::Col_TabLabelSelectedHovered]    = ColorConvertFloat4ToU32(colors[ImGuiCol_ButtonHovered]);
    style.colors[TabLabelStyle::Col_TabLabelSelectedActive]     = ColorConvertFloat4ToU32(colors[ImGuiCol_ButtonActive]);
    style.colors[TabLabelStyle::Col_TabLabelSelectedBorder]     = ColorConvertFloat4ToU32(colors[ImGuiCol_Border]);
    style.colors[TabLabelStyle::Col_TabLabelSelectedText]       = ColorConvertFloat4ToU32(ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
}

void ImGui::StyleColorsFurtive(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.71f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.06f, 0.06f, 0.06f, 0.01f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.10f, 0.10f, 0.10f, 0.71f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.20f, 0.20f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.10f, 0.10f, 0.10f, 0.66f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.37f, 0.00f, 0.12f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.44f, 0.00f, 0.13f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(1.00f, 0.00f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(1.00f, 0.00f, 0.30f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.00f, 0.33f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(1.00f, 0.00f, 0.33f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.00f, 0.36f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.98f, 0.00f, 0.26f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.98f, 0.00f, 0.26f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.98f, 0.00f, 0.26f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.30f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.30f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.00f, 0.00f, 0.30f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    StyleTabLabels(colors);
}

void ImGui::StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    StyleTabLabels(colors);
}

void ImGui::StyleColorsClassic(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    StyleTabLabels(colors);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void ImGui::StyleColorsLight(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.90f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    StyleTabLabels(colors);
}

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

ImDrawListSharedData::ImDrawListSharedData()
{
    Font = NULL;
    FontSize = 0.0f;
    CurveTessellationTol = 0.0f;
    CircleSegmentMaxError = 0.0f;
    ClipRectFullscreen = ImVec4(-8192.0f, -8192.0f, +8192.0f, +8192.0f);
    InitialFlags = ImDrawListFlags_None;

    // Lookup tables
    for (int i = 0; i < IM_ARRAYSIZE(ArcFastVtx); i++)
    {
        const float a = ((float)i * 2 * IM_PI) / (float)IM_ARRAYSIZE(ArcFastVtx);
        ArcFastVtx[i] = ImVec2(ImCos(a), ImSin(a));
    }
    memset(CircleSegmentCounts, 0, sizeof(CircleSegmentCounts)); // This will be set by SetCircleSegmentMaxError()
}

void ImDrawListSharedData::SetCircleSegmentMaxError(float max_error)
{
    if (CircleSegmentMaxError == max_error)
        return;
    CircleSegmentMaxError = max_error;
    for (int i = 0; i < IM_ARRAYSIZE(CircleSegmentCounts); i++)
    {
        const float radius = i + 1.0f;
        const int segment_count = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, CircleSegmentMaxError);
        CircleSegmentCounts[i] = (ImU8)ImMin(segment_count, 255);
    }
}

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    Flags = _Data ? _Data->InitialFlags : ImDrawListFlags_None;
    _VtxCurrentOffset = 0;
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _Splitter.Clear();
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _Splitter.ClearFreeMemory();
}

ImDrawList* ImDrawList::CloneOutput() const
{
    ImDrawList* dst = IM_NEW(ImDrawList(_Data));
    dst->CmdBuffer = CmdBuffer;
    dst->IdxBuffer = IdxBuffer;
    dst->VtxBuffer = VtxBuffer;
    dst->Flags = Flags;
    return dst;
}

// Using macros because C++ is a terrible language, we want guaranteed inline, no code in header, and no overhead in Debug builds
#define GetCurrentClipRect()    (_ClipRectStack.Size ? _ClipRectStack.Data[_ClipRectStack.Size-1]  : _Data->ClipRectFullscreen)
#define GetCurrentTextureId()   (_TextureIdStack.Size ? _TextureIdStack.Data[_TextureIdStack.Size-1] : (ImTextureID)NULL)

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = GetCurrentClipRect();
    draw_cmd.TextureId = GetCurrentTextureId();
    draw_cmd.VtxOffset = _VtxCurrentOffset;
    draw_cmd.IdxOffset = IdxBuffer.Size;

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::UpdateClipRect()
{
    // If current command is used with different settings we need to add a new command
    const ImVec4 curr_clip_rect = GetCurrentClipRect();
    ImDrawCmd* curr_cmd = CmdBuffer.Size > 0 ? &CmdBuffer.Data[CmdBuffer.Size-1] : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) != 0) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) == 0 && prev_cmd->TextureId == GetCurrentTextureId() && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->ClipRect = curr_clip_rect;
}

void ImDrawList::UpdateTextureID()
{
    // If current command is used with different settings we need to add a new command
    const ImTextureID curr_texture_id = GetCurrentTextureId();
    ImDrawCmd* curr_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != curr_texture_id) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && prev_cmd->TextureId == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(ImVec4)) == 0 && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->TextureId = curr_texture_id;
}

#undef GetCurrentClipRect
#undef GetCurrentTextureId

// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(ImVec2 cr_min, ImVec2 cr_max, bool intersect_with_current_clip_rect)
{
    ImVec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if (intersect_with_current_clip_rect && _ClipRectStack.Size)
    {
        ImVec4 current = _ClipRectStack.Data[_ClipRectStack.Size-1];
        if (cr.x < current.x) cr.x = current.x;
        if (cr.y < current.y) cr.y = current.y;
        if (cr.z > current.z) cr.z = current.z;
        if (cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    UpdateClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(ImVec2(_Data->ClipRectFullscreen.x, _Data->ClipRectFullscreen.y), ImVec2(_Data->ClipRectFullscreen.z, _Data->ClipRectFullscreen.w));
}

void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}

void ImDrawList::PushTextureID(ImTextureID texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}

void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}

// Reserve space for a number of vertices and indices.
// You must finish filling your reserved data before calling PrimReserve() again, as it may reallocate or
// submit the intermediate results. PrimUnreserve() can be used to release unused allocations.
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    // Large mesh support (when enabled)
    IM_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);
    if (sizeof(ImDrawIdx) == 2 && (_VtxCurrentIdx + vtx_count >= (1 << 16)) && (Flags & ImDrawListFlags_AllowVtxOffset))
    {
        _VtxCurrentOffset = VtxBuffer.Size;
        _VtxCurrentIdx = 0;
        AddDrawCmd();
    }

    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd.ElemCount += idx_count;

    int vtx_buffer_old_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_old_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_old_size;

    int idx_buffer_old_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_old_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_old_size;
}

// Release the a number of reserved vertices/indices from the end of the last reservation made with PrimReserve().
void ImDrawList::PrimUnreserve(int idx_count, int vtx_count)
{
    IM_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);

    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd.ElemCount -= idx_count;
    VtxBuffer.shrink(VtxBuffer.Size - vtx_count);
    IdxBuffer.shrink(IdxBuffer.Size - idx_count);
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(_Data->TexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// On AddPolyline() and AddConvexPolyFilled() we intentionally avoid using ImVec2 and superflous function calls to optimize debug/non-inlined builds.
// Those macros expects l-values.
#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     do { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / ImSqrt(d2); VX *= inv_len; VY *= inv_len; } } while (0)
#define IM_FIXNORMAL2F(VX,VY)               do { float d2 = VX*VX + VY*VY; if (d2 < 0.5f) d2 = 0.5f; float inv_lensq = 1.0f / d2; VX *= inv_lensq; VY *= inv_lensq; } while (0)

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We avoid using the ImVec2 math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness)
{
    if (points_count < 2)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;

    int count = points_count;
    if (!closed)
        count = points_count-1;

    const bool thick_line = thickness > 1.0f;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;

        const int idx_count = thick_line ? count*18 : count*12;
        const int vtx_count = thick_line ? points_count*4 : points_count*3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2)); //-V630
        ImVec2* temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1;
            float dx = points[i2].x - points[i1].x;
            float dy = points[i2].y - points[i1].y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i1].x = dy;
            temp_normals[i1].y = -dx;
        }
        if (!closed)
            temp_normals[points_count-1] = temp_normals[points_count-2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count-1)*2+0] = points[points_count-1] + temp_normals[points_count-1] * AA_SIZE;
                temp_points[(points_count-1)*2+1] = points[points_count-1] - temp_normals[points_count-1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+3;

                // Average normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                dm_x *= AA_SIZE;
                dm_y *= AA_SIZE;

                // Add temporary vertexes
                ImVec2* out_vtx = &temp_points[i2*2];
                out_vtx[0].x = points[i2].x + dm_x;
                out_vtx[0].y = points[i2].y + dm_y;
                out_vtx[1].x = points[i2].x - dm_x;
                out_vtx[1].y = points[i2].y - dm_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2+0); _IdxWritePtr[1] = (ImDrawIdx)(idx1+0); _IdxWritePtr[2] = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1+2); _IdxWritePtr[4] = (ImDrawIdx)(idx2+2); _IdxWritePtr[5] = (ImDrawIdx)(idx2+0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2+1); _IdxWritePtr[7] = (ImDrawIdx)(idx1+1); _IdxWritePtr[8] = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1+0); _IdxWritePtr[10]= (ImDrawIdx)(idx2+0); _IdxWritePtr[11]= (ImDrawIdx)(idx2+1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i*2+0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i*2+1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        }
        else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+0] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+1] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+2] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+3] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+4;

                // Average normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
                float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
                float dm_in_x = dm_x * half_inner_thickness;
                float dm_in_y = dm_y * half_inner_thickness;

                // Add temporary vertexes
                ImVec2* out_vtx = &temp_points[i2*4];
                out_vtx[0].x = points[i2].x + dm_out_x;
                out_vtx[0].y = points[i2].y + dm_out_y;
                out_vtx[1].x = points[i2].x + dm_in_x;
                out_vtx[1].y = points[i2].y + dm_in_y;
                out_vtx[2].x = points[i2].x - dm_in_x;
                out_vtx[2].y = points[i2].y - dm_in_y;
                out_vtx[3].x = points[i2].x - dm_out_x;
                out_vtx[3].y = points[i2].y - dm_out_y;

                // Add indexes
                _IdxWritePtr[0]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[1]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[2]  = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3]  = (ImDrawIdx)(idx1+2); _IdxWritePtr[4]  = (ImDrawIdx)(idx2+2); _IdxWritePtr[5]  = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[6]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[7]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[8]  = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9]  = (ImDrawIdx)(idx1+0); _IdxWritePtr[10] = (ImDrawIdx)(idx2+0); _IdxWritePtr[11] = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2+2); _IdxWritePtr[13] = (ImDrawIdx)(idx1+2); _IdxWritePtr[14] = (ImDrawIdx)(idx1+3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1+3); _IdxWritePtr[16] = (ImDrawIdx)(idx2+3); _IdxWritePtr[17] = (ImDrawIdx)(idx2+2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i*4+0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i*4+1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i*4+2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i*4+3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count*6;
        const int vtx_count = count*4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            dx *= (thickness * 0.5f);
            dy *= (thickness * 0.5f);

            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx+2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx+3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

// We intentionally avoid using ImVec2 and its math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col)
{
    if (points_count < 3)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;

    if (Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count-2)*3 + points_count*6;
        const int vtx_count = (points_count*2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx+1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+((i-1)<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx+(i<<1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2)); //-V630
        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            float dm_x = (n0.x + n1.x) * 0.5f;
            float dm_y = (n0.y + n1.y) * 0.5f;
            IM_FIXNORMAL2F(dm_x, dm_y);
            dm_x *= AA_SIZE * 0.5f;
            dm_y *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos.x = (points[i1].x - dm_x); _VtxWritePtr[0].pos.y = (points[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos.x = (points[i1].x + dm_x); _VtxWritePtr[1].pos.y = (points[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx+(i1<<1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+(i0<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx+(i0<<1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx+(i0<<1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx+(i1<<1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx+(i1<<1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count-2)*3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+i-1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius == 0.0f || a_min_of_12 > a_max_of_12)
    {
        _Path.push_back(center);
        return;
    }

    // For legacy reason the PathArcToFast() always takes angles where 2*PI is represented by 12,
    // but it is possible to set IM_DRAWLIST_ARCFAST_TESSELATION_MULTIPLIER to a higher value. This should compile to a no-op otherwise.
#if IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER != 1
    a_min_of_12 *= IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER;
    a_max_of_12 *= IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER;
#endif

    _Path.reserve(_Path.Size + (a_max_of_12 - a_min_of_12 + 1));
    for (int a = a_min_of_12; a <= a_max_of_12; a++)
    {
        const ImVec2& c = _Data->ArcFastVtx[a % IM_ARRAYSIZE(_Data->ArcFastVtx)];
        _Path.push_back(ImVec2(center.x + c.x * radius, center.y + c.y * radius));
    }
}

void ImDrawList::PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    if (radius == 0.0f)
    {
        _Path.push_back(center);
        return;
    }

    // Note that we are adding a point at both a_min and a_max.
    // If you are trying to draw a full closed circle you don't want the overlapping points!
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Path.push_back(ImVec2(center.x + ImCos(a) * radius, center.y + ImSin(a) * radius));
    }
}

ImVec2 ImBezierCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
{
    float u = 1.0f - t;
    float w1 = u*u*u;
    float w2 = 3*u*u*t;
    float w3 = 3*u*t*t;
    float w4 = t*t*t;
    return ImVec2(w1*p1.x + w2*p2.x + w3*p3.x + w4*p4.x, w1*p1.y + w2*p2.y + w3*p3.y + w4*p4.y);
}

// Closely mimics BezierClosestPointCasteljauStep() in imgui.cpp
static void PathBezierToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2+d3) * (d2+d3) < tess_tol * (dx*dx + dy*dy))
    {
        path->push_back(ImVec2(x4, y4));
    }
    else if (level < 10)
    {
        float x12 = (x1+x2)*0.5f,       y12 = (y1+y2)*0.5f;
        float x23 = (x2+x3)*0.5f,       y23 = (y2+y3)*0.5f;
        float x34 = (x3+x4)*0.5f,       y34 = (y3+y4)*0.5f;
        float x123 = (x12+x23)*0.5f,    y123 = (y12+y23)*0.5f;
        float x234 = (x23+x34)*0.5f,    y234 = (y23+y34)*0.5f;
        float x1234 = (x123+x234)*0.5f, y1234 = (y123+y234)*0.5f;
        PathBezierToCasteljau(path, x1,y1,        x12,y12,    x123,y123,  x1234,y1234, tess_tol, level+1);
        PathBezierToCasteljau(path, x1234,y1234,  x234,y234,  x34,y34,    x4,y4,       tess_tol, level+1);
    }
}

void ImDrawList::PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, _Data->CurveTessellationTol, 0); // Auto-tessellated
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
            _Path.push_back(ImBezierCalc(p1, p2, p3, p4, t_step * i_step));
    }
}

void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, ImDrawCornerFlags rounding_corners)
{
    rounding = ImMin(rounding, ImFabs(b.x - a.x) * ( ((rounding_corners & ImDrawCornerFlags_Top)  == ImDrawCornerFlags_Top)  || ((rounding_corners & ImDrawCornerFlags_Bot)   == ImDrawCornerFlags_Bot)   ? 0.5f : 1.0f ) - 1.0f);
    rounding = ImMin(rounding, ImFabs(b.y - a.y) * ( ((rounding_corners & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((rounding_corners & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f ) - 1.0f);

    if (rounding <= 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x, a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x, b.y));
    }
    else
    {
        const float rounding_tl = (rounding_corners & ImDrawCornerFlags_TopLeft) ? rounding : 0.0f;
        const float rounding_tr = (rounding_corners & ImDrawCornerFlags_TopRight) ? rounding : 0.0f;
        const float rounding_br = (rounding_corners & ImDrawCornerFlags_BotRight) ? rounding : 0.0f;
        const float rounding_bl = (rounding_corners & ImDrawCornerFlags_BotLeft) ? rounding : 0.0f;
        PathArcToFast(ImVec2(a.x + rounding_tl, a.y + rounding_tl), rounding_tl, 6, 9);
        PathArcToFast(ImVec2(b.x - rounding_tr, a.y + rounding_tr), rounding_tr, 9, 12);
        PathArcToFast(ImVec2(b.x - rounding_br, b.y - rounding_br), rounding_br, 0, 3);
        PathArcToFast(ImVec2(a.x + rounding_bl, b.y - rounding_bl), rounding_bl, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1 + ImVec2(0.5f, 0.5f));
    PathLineTo(p2 + ImVec2(0.5f, 0.5f));
    PathStroke(col, false, thickness);
}

// p_min = upper-left, p_max = lower-right
// Note we don't render 1 pixels sized rectangles properly.
void ImDrawList::AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
        PathRect(p_min + ImVec2(0.50f,0.50f), p_max - ImVec2(0.50f,0.50f), rounding, rounding_corners);
    else
        PathRect(p_min + ImVec2(0.50f,0.50f), p_max - ImVec2(0.49f,0.49f), rounding, rounding_corners); // Better looking lower-right corner and rounded non-AA shapes.
    PathStroke(col, true, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(p_min, p_max, rounding, rounding_corners);
        PathFillConvex(col);
    }
    else
    {
        PrimReserve(6, 4);
        PrimRect(p_min, p_max, col);
    }
}

// p_min = upper-left, p_max = lower-right
void ImDrawList::AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+3));
    PrimWriteVtx(p_min, uv, col_upr_left);
    PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    PrimWriteVtx(p_max, uv, col_bot_right);
    PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}

void ImDrawList::AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathFillConvex(col);
}

void ImDrawList::AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathFillConvex(col);
}

void ImDrawList::AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    // Obtain segment count
    if (num_segments <= 0)
    {
        // Automatic segment count
        const int radius_idx = (int)radius - 1;
        if (radius_idx < IM_ARRAYSIZE(_Data->CircleSegmentCounts))
            num_segments = _Data->CircleSegmentCounts[radius_idx]; // Use cached value
        else
            num_segments = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, _Data->CircleSegmentMaxError);
    }
    else
    {
        // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
        num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);
    }

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    if (num_segments == 12)
        PathArcToFast(center, radius - 0.5f, 0, 12);
    else
        PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    // Obtain segment count
    if (num_segments <= 0)
    {
        // Automatic segment count
        const int radius_idx = (int)radius - 1;
        if (radius_idx < IM_ARRAYSIZE(_Data->CircleSegmentCounts))
            num_segments = _Data->CircleSegmentCounts[radius_idx]; // Use cached value
        else
            num_segments = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, _Data->CircleSegmentMaxError);
    }
    else
    {
        // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
        num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);
    }

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    if (num_segments == 12)
        PathArcToFast(center, radius, 0, 12);
    else
        PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

// Guaranteed to honor 'num_segments'
void ImDrawList::AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, true, thickness);
}

// Guaranteed to honor 'num_segments'
void ImDrawList::AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

// Cubic Bezier takes 4 controls points
void ImDrawList::AddBezierCurve(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathBezierCurveTo(p2, p3, p4, num_segments);
    PathStroke(col, false, thickness);
}

void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared ImDrawListSharedData instance
    if (font == NULL)
        font = _Data->Font;
    if (font_size == 0.0f)
        font_size = _Data->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    ImVec4 clip_rect = _ClipRectStack.back();
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}

void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(p_min, p_max, uv_min, uv_max, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimQuadUV(p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (rounding <= 0.0f || (rounding_corners & ImDrawCornerFlags_All) == 0)
    {
        AddImage(user_texture_id, p_min, p_max, uv_min, uv_max, col);
        return;
    }

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    int vert_start_idx = VtxBuffer.Size;
    PathRect(p_min, p_max, rounding, rounding_corners);
    PathFillConvex(col);
    int vert_end_idx = VtxBuffer.Size;
    ImGui::ShadeVertsLinearUV(this, vert_start_idx, vert_end_idx, p_min, p_max, uv_min, uv_max, true);

    if (push_texture_id)
        PopTextureID();
}


//-----------------------------------------------------------------------------
// ImDrawListSplitter
//-----------------------------------------------------------------------------
// FIXME: This may be a little confusing, trying to be a little too low-level/optimal instead of just doing vector swap..
//-----------------------------------------------------------------------------

void ImDrawListSplitter::ClearFreeMemory()
{
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == _Current)
            memset(&_Channels[i], 0, sizeof(_Channels[i]));  // Current channel is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i]._CmdBuffer.clear();
        _Channels[i]._IdxBuffer.clear();
    }
    _Current = 0;
    _Count = 1;
    _Channels.clear();
}

void ImDrawListSplitter::Split(ImDrawList* draw_list, int channels_count)
{
    IM_ASSERT(_Current == 0 && _Count <= 1 && "Nested channel splitting is not supported. Please use separate instances of ImDrawListSplitter.");
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _Count = channels_count;

    // Channels[] (24/32 bytes each) hold storage that we'll swap with draw_list->_CmdBuffer/_IdxBuffer
    // The content of Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy draw_list->_CmdBuffer/_IdxBuffer into Channels[0] and then Channels[1] into draw_list->CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        }
        else
        {
            _Channels[i]._CmdBuffer.resize(0);
            _Channels[i]._IdxBuffer.resize(0);
        }
        if (_Channels[i]._CmdBuffer.Size == 0)
        {
            ImDrawCmd draw_cmd;
            draw_cmd.ClipRect = draw_list->_ClipRectStack.back();
            draw_cmd.TextureId = draw_list->_TextureIdStack.back();
            _Channels[i]._CmdBuffer.push_back(draw_cmd);
        }
    }
}

static inline bool CanMergeDrawCommands(ImDrawCmd* a, ImDrawCmd* b)
{
    return memcmp(&a->ClipRect, &b->ClipRect, sizeof(a->ClipRect)) == 0 && a->TextureId == b->TextureId && a->VtxOffset == b->VtxOffset && !a->UserCallback && !b->UserCallback;
}

void ImDrawListSplitter::Merge(ImDrawList* draw_list)
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_Count <= 1)
        return;

    SetCurrentChannel(draw_list, 0);
    if (draw_list->CmdBuffer.Size != 0 && draw_list->CmdBuffer.back().ElemCount == 0)
        draw_list->CmdBuffer.pop_back();

    // Calculate our final buffer sizes. Also fix the incorrect IdxOffset values in each command.
    int new_cmd_buffer_count = 0;
    int new_idx_buffer_count = 0;
    ImDrawCmd* last_cmd = (_Count > 0 && draw_list->CmdBuffer.Size > 0) ? &draw_list->CmdBuffer.back() : NULL;
    int idx_offset = last_cmd ? last_cmd->IdxOffset + last_cmd->ElemCount : 0;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch._CmdBuffer.Size > 0 && ch._CmdBuffer.back().ElemCount == 0)
            ch._CmdBuffer.pop_back();
        if (ch._CmdBuffer.Size > 0 && last_cmd != NULL && CanMergeDrawCommands(last_cmd, &ch._CmdBuffer[0]))
        {
            // Merge previous channel last draw command with current channel first draw command if matching.
            last_cmd->ElemCount += ch._CmdBuffer[0].ElemCount;
            idx_offset += ch._CmdBuffer[0].ElemCount;
            ch._CmdBuffer.erase(ch._CmdBuffer.Data); // FIXME-OPT: Improve for multiple merges.
        }
        if (ch._CmdBuffer.Size > 0)
            last_cmd = &ch._CmdBuffer.back();
        new_cmd_buffer_count += ch._CmdBuffer.Size;
        new_idx_buffer_count += ch._IdxBuffer.Size;
        for (int cmd_n = 0; cmd_n < ch._CmdBuffer.Size; cmd_n++)
        {
            ch._CmdBuffer.Data[cmd_n].IdxOffset = idx_offset;
            idx_offset += ch._CmdBuffer.Data[cmd_n].ElemCount;
        }
    }
    draw_list->CmdBuffer.resize(draw_list->CmdBuffer.Size + new_cmd_buffer_count);
    draw_list->IdxBuffer.resize(draw_list->IdxBuffer.Size + new_idx_buffer_count);

    // Write commands and indices in order (they are fairly small structures, we don't copy vertices only indices)
    ImDrawCmd* cmd_write = draw_list->CmdBuffer.Data + draw_list->CmdBuffer.Size - new_cmd_buffer_count;
    ImDrawIdx* idx_write = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch._CmdBuffer.Size) { memcpy(cmd_write, ch._CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch._IdxBuffer.Size) { memcpy(idx_write, ch._IdxBuffer.Data, sz * sizeof(ImDrawIdx)); idx_write += sz; }
    }
    draw_list->_IdxWritePtr = idx_write;
    draw_list->UpdateClipRect(); // We call this instead of AddDrawCmd(), so that empty channels won't produce an extra draw call.
    draw_list->UpdateTextureID();
    _Count = 1;
}

void ImDrawListSplitter::SetCurrentChannel(ImDrawList* draw_list, int idx)
{
    IM_ASSERT(idx >= 0 && idx < _Count);
    if (_Current == idx)
        return;
    // Overwrite ImVector (12/16 bytes), four times. This is merely a silly optimization instead of doing .swap()
    memcpy(&_Channels.Data[_Current]._CmdBuffer, &draw_list->CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&_Channels.Data[_Current]._IdxBuffer, &draw_list->IdxBuffer, sizeof(draw_list->IdxBuffer));
    _Current = idx;
    memcpy(&draw_list->CmdBuffer, &_Channels.Data[idx]._CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&draw_list->IdxBuffer, &_Channels.Data[idx]._IdxBuffer, sizeof(draw_list->IdxBuffer));
    draw_list->_IdxWritePtr = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size;
}

//-----------------------------------------------------------------------------
// [SECTION] ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

// Helper to scale the ClipRect field of each ImDrawCmd.
// Use if your final output buffer is at a different scale than draw_data->DisplaySize,
// or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const ImVec2& fb_scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * fb_scale.x, cmd->ClipRect.y * fb_scale.y, cmd->ClipRect.z * fb_scale.x, cmd->ClipRect.w * fb_scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers ShadeVertsXXX functions
//-----------------------------------------------------------------------------

// Generic linear color gradient, write to RGB fields, leave A untouched.
void ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = ImLerp((int)(col0 >> IM_COL32_R_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_R_SHIFT) & 0xFF, t);
        int g = ImLerp((int)(col0 >> IM_COL32_G_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_G_SHIFT) & 0xFF, t);
        int b = ImLerp((int)(col0 >> IM_COL32_B_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_B_SHIFT) & 0xFF, t);
        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
    }
}

// Distribute UV over (a, b) rectangle
void ImGui::ShadeVertsLinearUV(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp)
{
    const ImVec2 size = b - a;
    const ImVec2 uv_size = uv_b - uv_a;
    const ImVec2 scale = ImVec2(
        size.x != 0.0f ? (uv_size.x / size.x) : 0.0f,
        size.y != 0.0f ? (uv_size.y / size.y) : 0.0f);

    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    if (clamp)
    {
        const ImVec2 min = ImMin(uv_a, uv_b);
        const ImVec2 max = ImMax(uv_a, uv_b);
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = ImClamp(uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale), min, max);
    }
    else
    {
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontConfig
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    FontData = NULL;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 3; // FIXME: 2 may be a better default?
    OversampleV = 1;
    PixelSnapH = false;
    GlyphExtraSpacing = ImVec2(0.0f, 0.0f);
    GlyphOffset = ImVec2(0.0f, 0.0f);
    GlyphRanges = NULL;
    GlyphMinAdvanceX = 0.0f;
    GlyphMaxAdvanceX = FLT_MAX;
    MergeMode = false;
    RasterizerFlags = 0x00;
    RasterizerMultiply = 1.0f;
    EllipsisChar = (ImWchar)-1;
    memset(Name, 0, sizeof(Name));
    DstFont = NULL;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The white texels on the top left are the ones we'll use everywhere in Dear ImGui to render filled shapes.
const int FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF = 108;
const int FONT_ATLAS_DEFAULT_TEX_DATA_H      = 27;
const unsigned int FONT_ATLAS_DEFAULT_TEX_DATA_ID = 0x80000000;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * FONT_ATLAS_DEFAULT_TEX_DATA_H + 1] =
{
    "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          "
    "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         "
    "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         "
    "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         "
    "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         "
    "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       "
    "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    "
    "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  "
    "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X "
    "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X"
    "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X"
    "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X"
    "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X"
    "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X"
    "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X"
    "X....XXXXXX -XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X"
    "X...X       ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X "
    "X..X        -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X "
    "X.X         -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X "
    "XX          -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  "
    "                    -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  "
    "                    -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  "
    "------------        -    X    -           X           -X.....................X-           ------------------"
    "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                             "
    "                                                      -  X..X           X..X  -                             "
    "                                                      -   X.X           X.X   -                             "
    "                                                      -    XX           XX    -                             "
};

static const ImVec2 FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[ImGuiMouseCursor_COUNT][3] =
{
    // Pos ........ Size ......... Offset ......
    { ImVec2( 0,3), ImVec2(12,19), ImVec2( 0, 0) }, // ImGuiMouseCursor_Arrow
    { ImVec2(13,0), ImVec2( 7,16), ImVec2( 1, 8) }, // ImGuiMouseCursor_TextInput
    { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_ResizeAll
    { ImVec2(21,0), ImVec2( 9,23), ImVec2( 4,11) }, // ImGuiMouseCursor_ResizeNS
    { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 4) }, // ImGuiMouseCursor_ResizeEW
    { ImVec2(73,0), ImVec2(17,17), ImVec2( 8, 8) }, // ImGuiMouseCursor_ResizeNESW
    { ImVec2(55,0), ImVec2(17,17), ImVec2( 8, 8) }, // ImGuiMouseCursor_ResizeNWSE
    { ImVec2(91,0), ImVec2(17,22), ImVec2( 5, 0) }, // ImGuiMouseCursor_Hand
};

ImFontAtlas::ImFontAtlas()
{
    Locked = false;
    Flags = ImFontAtlasFlags_None;
    TexID = (ImTextureID)NULL;
    TexDesiredWidth = 0;
    TexGlyphPadding = 1;

    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = 0;
    TexUvScale = ImVec2(0.0f, 0.0f);
    TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

ImFontAtlas::~ImFontAtlas()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            IM_FREE(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
        {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
    CustomRects.clear();
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

void    ImFontAtlas::ClearTexData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    if (TexPixelsAlpha8)
        IM_FREE(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        IM_FREE(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < Fonts.Size; i++)
        IM_DELETE(Fonts[i]);
    Fonts.clear();
}

void    ImFontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build atlas on demand
    if (TexPixelsAlpha8 == NULL)
    {
        if (ConfigData.empty())
            AddFontDefault();
        Build();
    }

    *out_pixels = TexPixelsAlpha8;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels = NULL;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        if (pixels)
        {
            TexPixelsRGBA32 = (unsigned int*)IM_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
            const unsigned char* src = pixels;
            unsigned int* dst = TexPixelsRGBA32;
            for (int n = TexWidth * TexHeight; n > 0; n--)
                *dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
        }
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
        Fonts.push_back(IM_NEW(ImFont));
    else
        IM_ASSERT(!Fonts.empty() && "Cannot use MergeMode for the first font"); // When using MergeMode make sure that a font has already been added before. You can use ImGui::GetIO().Fonts->AddFontDefault() to add the default imgui font.

    ConfigData.push_back(*font_cfg);
    ImFontConfig& new_font_cfg = ConfigData.back();
    if (new_font_cfg.DstFont == NULL)
        new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = IM_ALLOC(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    if (new_font_cfg.DstFont->EllipsisChar == (ImWchar)-1)
        new_font_cfg.DstFont->EllipsisChar = font_cfg->EllipsisChar;

    // Invalidate texture
    ClearTexData();
    return new_font_cfg.DstFont;
}

// Default font TTF is compressed with stb_compress then base85 encoded (see misc/fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(const unsigned char *input);
static unsigned int stb_decompress(unsigned char *output, const unsigned char *input, unsigned int length);
static const char*  GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c)                                    { return c >= '\\' ? c-36 : c-35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0]) + 85*(Decode85Byte(src[1]) + 85*(Decode85Byte(src[2]) + 85*(Decode85Byte(src[3]) + 85*Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont* ImFontAtlas::AddFontDefault(const ImFontConfig* font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.SizePixels <= 0.0f)
        font_cfg.SizePixels = 14.0f * 1.0f;
    if (font_cfg.Name[0] == '\0')
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "ProggyClean.ttf, %dpx", (int)font_cfg.SizePixels);
    font_cfg.EllipsisChar = (ImWchar)0x0085;

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    const ImWchar* glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges : GetGlyphRangesDefault();
    ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, font_cfg.SizePixels, &font_cfg, glyph_ranges);
    font->DisplayOffset.y = 0.0f;
    return font;
}

ImFont* ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    size_t data_size = 0;
    void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        IM_ASSERT_USER_ERROR(0, "Could not load font file!");
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", p, size_pixels);
    }
    return AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NB: Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels;
    if (glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((const unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char *)IM_ALLOC(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (const unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = IM_ALLOC((size_t)compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
    ImFont* font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    IM_FREE(compressed_ttf);
    return font;
}

int ImFontAtlas::AddCustomRectRegular(unsigned int id, int width, int height)
{
    // Breaking change on 2019/11/21 (1.74): ImFontAtlas::AddCustomRectRegular() now requires an ID >= 0x110000 (instead of >= 0x10000)
    IM_ASSERT(id >= 0x110000);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    ImFontAtlasCustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

int ImFontAtlas::AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset)
{
    IM_ASSERT(font != NULL);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    ImFontAtlasCustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    r.GlyphAdvanceX = advance_x;
    r.GlyphOffset = offset;
    r.Font = font;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

void ImFontAtlas::CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max) const
{
    IM_ASSERT(TexWidth > 0 && TexHeight > 0);   // Font atlas needs to be built before we can calculate UV coordinates
    IM_ASSERT(rect->IsPacked());                // Make sure the rectangle has been packed
    *out_uv_min = ImVec2((float)rect->X * TexUvScale.x, (float)rect->Y * TexUvScale.y);
    *out_uv_max = ImVec2((float)(rect->X + rect->Width) * TexUvScale.x, (float)(rect->Y + rect->Height) * TexUvScale.y);
}

bool ImFontAtlas::GetMouseCursorTexData(ImGuiMouseCursor cursor_type, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2])
{
    if (cursor_type <= ImGuiMouseCursor_None || cursor_type >= ImGuiMouseCursor_COUNT)
        return false;
    if (Flags & ImFontAtlasFlags_NoMouseCursors)
        return false;

    IM_ASSERT(CustomRectIds[0] != -1);
    ImFontAtlasCustomRect& r = CustomRects[CustomRectIds[0]];
    IM_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    ImVec2 pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][0] + ImVec2((float)r.X, (float)r.Y);
    ImVec2 size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][1];
    *out_size = size;
    *out_offset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][2];
    out_uv_border[0] = (pos) * TexUvScale;
    out_uv_border[1] = (pos + size) * TexUvScale;
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
    out_uv_fill[0] = (pos) * TexUvScale;
    out_uv_fill[1] = (pos + size) * TexUvScale;
    return true;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    return ImFontAtlasBuildWithStbTruetype(this);
}

void    ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_brighten_factor)
{
    for (unsigned int i = 0; i < 256; i++)
    {
        unsigned int value = (unsigned int)(i * in_brighten_factor);
        out_table[i] = value > 255 ? 255 : (value & 0xFF);
    }
}

void    ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride)
{
    unsigned char* data = pixels + x + y * stride;
    for (int j = h; j > 0; j--, data += stride)
        for (int i = 0; i < w; i++)
            data[i] = table[data[i]];
}

// Temporary data for one source font (multiple source fonts can be merged into one destination ImFont)
// (C++03 doesn't allow instancing ImVector<> with function-local types so we declare the type here.)
struct ImFontBuildSrcData
{
    stbtt_fontinfo      FontInfo;
    stbtt_pack_range    PackRange;          // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
    stbrp_rect*         Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
    stbtt_packedchar*   PackedChars;        // Output glyphs
    const ImWchar*      SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
    int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
    int                 GlyphsHighest;      // Highest requested codepoint
    int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
    ImBitVector         GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
    ImVector<int>       GlyphsList;         // Glyph codepoints list (flattened version of GlyphsMap)
};

// Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
struct ImFontBuildDstData
{
    int                 SrcCount;           // Number of source fonts targeting this destination font.
    int                 GlyphsHighest;
    int                 GlyphsCount;
    ImBitVector         GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
};

static void UnpackBitVectorToFlatIndexList(const ImBitVector* in, ImVector<int>* out)
{
    IM_ASSERT(sizeof(in->Storage.Data[0]) == sizeof(int));
    const ImU32* it_begin = in->Storage.begin();
    const ImU32* it_end = in->Storage.end();
    for (const ImU32* it = it_begin; it < it_end; it++)
        if (ImU32 entries_32 = *it)
            for (ImU32 bit_n = 0; bit_n < 32; bit_n++)
                if (entries_32 & ((ImU32)1 << bit_n))
                    out->push_back((int)(((it - it_begin) << 5) + bit_n));
}

bool    ImFontAtlasBuildWithStbTruetype(ImFontAtlas* atlas)
{
    IM_ASSERT(atlas->ConfigData.Size > 0);

    ImFontAtlasBuildInit(atlas);

    // Clear atlas
    atlas->TexID = (ImTextureID)NULL;
    atlas->TexWidth = atlas->TexHeight = 0;
    atlas->TexUvScale = ImVec2(0.0f, 0.0f);
    atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    atlas->ClearTexData();

    // Temporary storage for building
    ImVector<ImFontBuildSrcData> src_tmp_array;
    ImVector<ImFontBuildDstData> dst_tmp_array;
    src_tmp_array.resize(atlas->ConfigData.Size);
    dst_tmp_array.resize(atlas->Fonts.Size);
    memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
    memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

    // 1. Initialize font loading structure, check font data validity
    for (int src_i = 0; src_i < atlas->ConfigData.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

        // Find index from cfg.DstFont (we allow the user to set cfg.DstFont. Also it makes casual debugging nicer than when storing indices)
        src_tmp.DstIndex = -1;
        for (int output_i = 0; output_i < atlas->Fonts.Size && src_tmp.DstIndex == -1; output_i++)
            if (cfg.DstFont == atlas->Fonts[output_i])
                src_tmp.DstIndex = output_i;
        IM_ASSERT(src_tmp.DstIndex != -1); // cfg.DstFont not pointing within atlas->Fonts[] array?
        if (src_tmp.DstIndex == -1)
            return false;

        // Initialize helper structure for font loading and verify that the TTF/OTF data is correct
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0 && "FontData is incorrect, or FontNo cannot be found.");
        if (!stbtt_InitFont(&src_tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
            return false;

        // Measure highest codepoints
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.SrcRanges = cfg.GlyphRanges ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            src_tmp.GlyphsHighest = ImMax(src_tmp.GlyphsHighest, (int)src_range[1]);
        dst_tmp.SrcCount++;
        dst_tmp.GlyphsHighest = ImMax(dst_tmp.GlyphsHighest, src_tmp.GlyphsHighest);
    }

    // 2. For every requested codepoint, check for their presence in the font data, and handle redundancy or overlaps between source fonts to avoid unused glyphs.
    int total_glyphs_count = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.GlyphsSet.Create(src_tmp.GlyphsHighest + 1);
        if (dst_tmp.GlyphsSet.Storage.empty())
            dst_tmp.GlyphsSet.Create(dst_tmp.GlyphsHighest + 1);

        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            for (unsigned int codepoint = src_range[0]; codepoint <= src_range[1]; codepoint++)
            {
                if (dst_tmp.GlyphsSet.TestBit(codepoint))    // Don't overwrite existing glyphs. We could make this an option for MergeMode (e.g. MergeOverwrite==true)
                    continue;
                if (!stbtt_FindGlyphIndex(&src_tmp.FontInfo, codepoint))    // It is actually in the font?
                    continue;

                // Add to avail set/counters
                src_tmp.GlyphsCount++;
                dst_tmp.GlyphsCount++;
                src_tmp.GlyphsSet.SetBit(codepoint);
                dst_tmp.GlyphsSet.SetBit(codepoint);
                total_glyphs_count++;
            }
    }

    // 3. Unpack our bit map into a flat list (we now have all the Unicode points that we know are requested _and_ available _and_ not overlapping another)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        src_tmp.GlyphsList.reserve(src_tmp.GlyphsCount);
        UnpackBitVectorToFlatIndexList(&src_tmp.GlyphsSet, &src_tmp.GlyphsList);
        src_tmp.GlyphsSet.Clear();
        IM_ASSERT(src_tmp.GlyphsList.Size == src_tmp.GlyphsCount);
    }
    for (int dst_i = 0; dst_i < dst_tmp_array.Size; dst_i++)
        dst_tmp_array[dst_i].GlyphsSet.Clear();
    dst_tmp_array.clear();

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    // (We technically don't need to zero-clear buf_rects, but let's do it for the sake of sanity)
    ImVector<stbrp_rect> buf_rects;
    ImVector<stbtt_packedchar> buf_packedchars;
    buf_rects.resize(total_glyphs_count);
    buf_packedchars.resize(total_glyphs_count);
    memset(buf_rects.Data, 0, (size_t)buf_rects.size_in_bytes());
    memset(buf_packedchars.Data, 0, (size_t)buf_packedchars.size_in_bytes());

    // 4. Gather glyphs sizes so we can pack them in our virtual canvas.
    int total_surface = 0;
    int buf_rects_out_n = 0;
    int buf_packedchars_out_n = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        src_tmp.Rects = &buf_rects[buf_rects_out_n];
        src_tmp.PackedChars = &buf_packedchars[buf_packedchars_out_n];
        buf_rects_out_n += src_tmp.GlyphsCount;
        buf_packedchars_out_n += src_tmp.GlyphsCount;

        // Convert our ranges in the format stb_truetype wants
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        src_tmp.PackRange.font_size = cfg.SizePixels;
        src_tmp.PackRange.first_unicode_codepoint_in_range = 0;
        src_tmp.PackRange.array_of_unicode_codepoints = src_tmp.GlyphsList.Data;
        src_tmp.PackRange.num_chars = src_tmp.GlyphsList.Size;
        src_tmp.PackRange.chardata_for_range = src_tmp.PackedChars;
        src_tmp.PackRange.h_oversample = (unsigned char)cfg.OversampleH;
        src_tmp.PackRange.v_oversample = (unsigned char)cfg.OversampleV;

        // Gather the sizes of all rectangles we will need to pack (this loop is based on stbtt_PackFontRangesGatherRects)
        const float scale = (cfg.SizePixels > 0) ? stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels) : stbtt_ScaleForMappingEmToPixels(&src_tmp.FontInfo, -cfg.SizePixels);
        const int padding = atlas->TexGlyphPadding;
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsList.Size; glyph_i++)
        {
            int x0, y0, x1, y1;
            const int glyph_index_in_font = stbtt_FindGlyphIndex(&src_tmp.FontInfo, src_tmp.GlyphsList[glyph_i]);
            IM_ASSERT(glyph_index_in_font != 0);
            stbtt_GetGlyphBitmapBoxSubpixel(&src_tmp.FontInfo, glyph_index_in_font, scale * cfg.OversampleH, scale * cfg.OversampleV, 0, 0, &x0, &y0, &x1, &y1);
            src_tmp.Rects[glyph_i].w = (stbrp_coord)(x1 - x0 + padding + cfg.OversampleH - 1);
            src_tmp.Rects[glyph_i].h = (stbrp_coord)(y1 - y0 + padding + cfg.OversampleV - 1);
            total_surface += src_tmp.Rects[glyph_i].w * src_tmp.Rects[glyph_i].h;
        }
    }

    // We need a width for the skyline algorithm, any width!
    // The exact width doesn't really matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
    // User can override TexDesiredWidth and TexGlyphPadding if they wish, otherwise we use a simple heuristic to select the width based on expected surface.
    const int surface_sqrt = (int)ImSqrt((float)total_surface) + 1;
    atlas->TexHeight = 0;
    if (atlas->TexDesiredWidth > 0)
        atlas->TexWidth = atlas->TexDesiredWidth;
    else
        atlas->TexWidth = (surface_sqrt >= 4096*0.7f) ? 4096 : (surface_sqrt >= 2048*0.7f) ? 2048 : (surface_sqrt >= 1024*0.7f) ? 1024 : 512;

    // 5. Start packing
    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    const int TEX_HEIGHT_MAX = 1024 * 32;
    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, NULL, atlas->TexWidth, TEX_HEIGHT_MAX, 0, atlas->TexGlyphPadding, NULL);
    ImFontAtlasBuildPackCustomRects(atlas, spc.pack_info);

    // 6. Pack each source font. No rendering yet, we are working with rectangles in an infinitely tall texture at this point.
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbrp_pack_rects((stbrp_context*)spc.pack_info, src_tmp.Rects, src_tmp.GlyphsCount);

        // Extend texture height and mark missing glyphs as non-packed so we won't render them.
        // FIXME: We are not handling packing failure here (would happen if we got off TEX_HEIGHT_MAX or if a single if larger than TexWidth?)
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
            if (src_tmp.Rects[glyph_i].was_packed)
                atlas->TexHeight = ImMax(atlas->TexHeight, src_tmp.Rects[glyph_i].y + src_tmp.Rects[glyph_i].h);
    }

    // 7. Allocate texture
    atlas->TexHeight = (atlas->Flags & ImFontAtlasFlags_NoPowerOfTwoHeight) ? (atlas->TexHeight + 1) : ImUpperPowerOfTwo(atlas->TexHeight);
    atlas->TexUvScale = ImVec2(1.0f / atlas->TexWidth, 1.0f / atlas->TexHeight);
    atlas->TexPixelsAlpha8 = (unsigned char*)IM_ALLOC(atlas->TexWidth * atlas->TexHeight);
    memset(atlas->TexPixelsAlpha8, 0, atlas->TexWidth * atlas->TexHeight);
    spc.pixels = atlas->TexPixelsAlpha8;
    spc.height = atlas->TexHeight;

    // 8. Render/rasterize font characters into the texture
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbtt_PackFontRangesRenderIntoRects(&spc, &src_tmp.FontInfo, &src_tmp.PackRange, 1, src_tmp.Rects);

        // Apply multiply operator
        if (cfg.RasterizerMultiply != 1.0f)
        {
            unsigned char multiply_table[256];
            ImFontAtlasBuildMultiplyCalcLookupTable(multiply_table, cfg.RasterizerMultiply);
            stbrp_rect* r = &src_tmp.Rects[0];
            for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++, r++)
                if (r->was_packed)
                    ImFontAtlasBuildMultiplyRectAlpha8(multiply_table, atlas->TexPixelsAlpha8, r->x, r->y, r->w, r->h, atlas->TexWidth * 1);
        }
        src_tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    buf_rects.clear();

    // 9. Setup ImFont and glyphs for runtime
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFont* dst_font = cfg.DstFont; // We can have multiple input fonts writing into a same destination font (when using MergeMode=true)

        const float font_scale = stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&src_tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        const float ascent = ImFloor(unscaled_ascent * font_scale + ((unscaled_ascent > 0.0f) ? +1 : -1));
        const float descent = ImFloor(unscaled_descent * font_scale + ((unscaled_descent > 0.0f) ? +1 : -1));
        ImFontAtlasBuildSetupFont(atlas, dst_font, &cfg, ascent, descent);
        const float font_off_x = cfg.GlyphOffset.x;
        const float font_off_y = cfg.GlyphOffset.y + IM_ROUND(dst_font->Ascent);

        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
        {
            const int codepoint = src_tmp.GlyphsList[glyph_i];
            const stbtt_packedchar& pc = src_tmp.PackedChars[glyph_i];

            const float char_advance_x_org = pc.xadvance;
            const float char_advance_x_mod = ImClamp(char_advance_x_org, cfg.GlyphMinAdvanceX, cfg.GlyphMaxAdvanceX);
            float char_off_x = font_off_x;
            if (char_advance_x_org != char_advance_x_mod)
                char_off_x += cfg.PixelSnapH ? ImFloor((char_advance_x_mod - char_advance_x_org) * 0.5f) : (char_advance_x_mod - char_advance_x_org) * 0.5f;

            // Register glyph
            stbtt_aligned_quad q;
            float dummy_x = 0.0f, dummy_y = 0.0f;
            stbtt_GetPackedQuad(src_tmp.PackedChars, atlas->TexWidth, atlas->TexHeight, glyph_i, &dummy_x, &dummy_y, &q, 0);
            dst_font->AddGlyph((ImWchar)codepoint, q.x0 + char_off_x, q.y0 + font_off_y, q.x1 + char_off_x, q.y1 + font_off_y, q.s0, q.t0, q.s1, q.t1, char_advance_x_mod);
        }
    }

    // Cleanup temporary (ImVector doesn't honor destructor)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
        src_tmp_array[src_i].~ImFontBuildSrcData();

    ImFontAtlasBuildFinish(atlas);
    return true;
}

// Register default custom rectangles (this is called/shared by both the stb_truetype and the FreeType builder)
void ImFontAtlasBuildInit(ImFontAtlas* atlas)
{
    if (atlas->CustomRectIds[0] >= 0)
        return;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF*2+1, FONT_ATLAS_DEFAULT_TEX_DATA_H);
    else
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, 2, 2);
}

void ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent)
{
    if (!font_config->MergeMode)
    {
        font->ClearOutputData();
        font->FontSize = font_config->SizePixels;
        font->ConfigData = font_config;
        font->ContainerAtlas = atlas;
        font->Ascent = ascent;
        font->Descent = descent;
    }
    font->ConfigDataCount++;
}

void ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* stbrp_context_opaque)
{
    stbrp_context* pack_context = (stbrp_context*)stbrp_context_opaque;
    IM_ASSERT(pack_context != NULL);

    ImVector<ImFontAtlasCustomRect>& user_rects = atlas->CustomRects;
    IM_ASSERT(user_rects.Size >= 1); // We expect at least the default custom rects to be registered, else something went wrong.

    ImVector<stbrp_rect> pack_rects;
    pack_rects.resize(user_rects.Size);
    memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
    for (int i = 0; i < user_rects.Size; i++)
    {
        pack_rects[i].w = user_rects[i].Width;
        pack_rects[i].h = user_rects[i].Height;
    }
    stbrp_pack_rects(pack_context, &pack_rects[0], pack_rects.Size);
    for (int i = 0; i < pack_rects.Size; i++)
        if (pack_rects[i].was_packed)
        {
            user_rects[i].X = pack_rects[i].x;
            user_rects[i].Y = pack_rects[i].y;
            IM_ASSERT(pack_rects[i].w == user_rects[i].Width && pack_rects[i].h == user_rects[i].Height);
            atlas->TexHeight = ImMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
        }
}

static void ImFontAtlasBuildRenderDefaultTexData(ImFontAtlas* atlas)
{
    IM_ASSERT(atlas->CustomRectIds[0] >= 0);
    IM_ASSERT(atlas->TexPixelsAlpha8 != NULL);
    ImFontAtlasCustomRect& r = atlas->CustomRects[atlas->CustomRectIds[0]];
    IM_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    IM_ASSERT(r.IsPacked());

    const int w = atlas->TexWidth;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
    {
        // Render/copy pixels
        IM_ASSERT(r.Width == FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1 && r.Height == FONT_ATLAS_DEFAULT_TEX_DATA_H);
        for (int y = 0, n = 0; y < FONT_ATLAS_DEFAULT_TEX_DATA_H; y++)
            for (int x = 0; x < FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF; x++, n++)
            {
                const int offset0 = (int)(r.X + x) + (int)(r.Y + y) * w;
                const int offset1 = offset0 + FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
                atlas->TexPixelsAlpha8[offset0] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == '.' ? 0xFF : 0x00;
                atlas->TexPixelsAlpha8[offset1] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == 'X' ? 0xFF : 0x00;
            }
    }
    else
    {
        IM_ASSERT(r.Width == 2 && r.Height == 2);
        const int offset = (int)(r.X) + (int)(r.Y) * w;
        atlas->TexPixelsAlpha8[offset] = atlas->TexPixelsAlpha8[offset + 1] = atlas->TexPixelsAlpha8[offset + w] = atlas->TexPixelsAlpha8[offset + w + 1] = 0xFF;
    }
    atlas->TexUvWhitePixel = ImVec2((r.X + 0.5f) * atlas->TexUvScale.x, (r.Y + 0.5f) * atlas->TexUvScale.y);
}

void ImFontAtlasBuildFinish(ImFontAtlas* atlas)
{
    // Render into our custom data block
    ImFontAtlasBuildRenderDefaultTexData(atlas);

    // Register custom rectangle glyphs
    for (int i = 0; i < atlas->CustomRects.Size; i++)
    {
        const ImFontAtlasCustomRect& r = atlas->CustomRects[i];
        if (r.Font == NULL || r.ID >= 0x110000)
            continue;

        IM_ASSERT(r.Font->ContainerAtlas == atlas);
        ImVec2 uv0, uv1;
        atlas->CalcCustomRectUV(&r, &uv0, &uv1);
        r.Font->AddGlyph((ImWchar)r.ID, r.GlyphOffset.x, r.GlyphOffset.y, r.GlyphOffset.x + r.Width, r.GlyphOffset.y + r.Height, uv0.x, uv0.y, uv1.x, uv1.y, r.GlyphAdvanceX);
    }

    // Build all fonts lookup tables
    for (int i = 0; i < atlas->Fonts.Size; i++)
        if (atlas->Fonts[i]->DirtyLookupTables)
            atlas->Fonts[i]->BuildLookupTable();

    // Ellipsis character is required for rendering elided text. We prefer using U+2026 (horizontal ellipsis).
    // However some old fonts may contain ellipsis at U+0085. Here we auto-detect most suitable ellipsis character.
    // FIXME: Also note that 0x2026 is currently seldomly included in our font ranges. Because of this we are more likely to use three individual dots.
    for (int i = 0; i < atlas->Fonts.size(); i++)
    {
        ImFont* font = atlas->Fonts[i];
        if (font->EllipsisChar != (ImWchar)-1)
            continue;
        const ImWchar ellipsis_variants[] = { (ImWchar)0x2026, (ImWchar)0x0085 };
        for (int j = 0; j < IM_ARRAYSIZE(ellipsis_variants); j++)
            if (font->FindGlyphNoFallback(ellipsis_variants[j]) != NULL) // Verify glyph exists
            {
                font->EllipsisChar = ellipsis_variants[j];
                break;
            }
    }
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar*   ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD79D, // Korean characters
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesChineseFull()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint, const short* accumulative_offsets, int accumulative_offsets_count, ImWchar* out_ranges)
{
    for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2)
    {
        out_ranges[0] = out_ranges[1] = (ImWchar)(base_codepoint + accumulative_offsets[n]);
        base_codepoint += accumulative_offsets[n];
    }
    out_ranges[0] = 0;
}

//-------------------------------------------------------------------------
// [SECTION] ImFontAtlas glyph ranges helpers
//-------------------------------------------------------------------------

const ImWchar*  ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
    // Store 2500 regularly used characters for Simplified Chinese.
    // Sourced from https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
    // This table covers 97.97% of all characters used during the month in July, 1987.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,3,2,1,2,2,1,1,1,1,1,5,2,1,2,3,3,3,2,2,4,1,1,1,2,1,5,2,3,1,2,1,2,1,1,2,1,1,2,2,1,4,1,1,1,1,5,10,1,2,19,2,1,2,1,2,1,2,1,2,
        1,5,1,6,3,2,1,2,2,1,1,1,4,8,5,1,1,4,1,1,3,1,2,1,5,1,2,1,1,1,10,1,1,5,2,4,6,1,4,2,2,2,12,2,1,1,6,1,1,1,4,1,1,4,6,5,1,4,2,2,4,10,7,1,1,4,2,4,
        2,1,4,3,6,10,12,5,7,2,14,2,9,1,1,6,7,10,4,7,13,1,5,4,8,4,1,1,2,28,5,6,1,1,5,2,5,20,2,2,9,8,11,2,9,17,1,8,6,8,27,4,6,9,20,11,27,6,68,2,2,1,1,
        1,2,1,2,2,7,6,11,3,3,1,1,3,1,2,1,1,1,1,1,3,1,1,8,3,4,1,5,7,2,1,4,4,8,4,2,1,2,1,1,4,5,6,3,6,2,12,3,1,3,9,2,4,3,4,1,5,3,3,1,3,7,1,5,1,1,1,1,2,
        3,4,5,2,3,2,6,1,1,2,1,7,1,7,3,4,5,15,2,2,1,5,3,22,19,2,1,1,1,1,2,5,1,1,1,6,1,1,12,8,2,9,18,22,4,1,1,5,1,16,1,2,7,10,15,1,1,6,2,4,1,2,4,1,6,
        1,1,3,2,4,1,6,4,5,1,2,1,1,2,1,10,3,1,3,2,1,9,3,2,5,7,2,19,4,3,6,1,1,1,1,1,4,3,2,1,1,1,2,5,3,1,1,1,2,2,1,1,2,1,1,2,1,3,1,1,1,3,7,1,4,1,1,2,1,
        1,2,1,2,4,4,3,8,1,1,1,2,1,3,5,1,3,1,3,4,6,2,2,14,4,6,6,11,9,1,15,3,1,28,5,2,5,5,3,1,3,4,5,4,6,14,3,2,3,5,21,2,7,20,10,1,2,19,2,4,28,28,2,3,
        2,1,14,4,1,26,28,42,12,40,3,52,79,5,14,17,3,2,2,11,3,4,6,3,1,8,2,23,4,5,8,10,4,2,7,3,5,1,1,6,3,1,2,2,2,5,28,1,1,7,7,20,5,3,29,3,17,26,1,8,4,
        27,3,6,11,23,5,3,4,6,13,24,16,6,5,10,25,35,7,3,2,3,3,14,3,6,2,6,1,4,2,3,8,2,1,1,3,3,3,4,1,1,13,2,2,4,5,2,1,14,14,1,2,2,1,4,5,2,3,1,14,3,12,
        3,17,2,16,5,1,2,1,8,9,3,19,4,2,2,4,17,25,21,20,28,75,1,10,29,103,4,1,2,1,1,4,2,4,1,2,3,24,2,2,2,1,1,2,1,3,8,1,1,1,2,1,1,3,1,1,1,6,1,5,3,1,1,
        1,3,4,1,1,5,2,1,5,6,13,9,16,1,1,1,1,3,2,3,2,4,5,2,5,2,2,3,7,13,7,2,2,1,1,1,1,2,3,3,2,1,6,4,9,2,1,14,2,14,2,1,18,3,4,14,4,11,41,15,23,15,23,
        176,1,3,4,1,1,1,1,5,3,1,2,3,7,3,1,1,2,1,2,4,4,6,2,4,1,9,7,1,10,5,8,16,29,1,1,2,2,3,1,3,5,2,4,5,4,1,1,2,2,3,3,7,1,6,10,1,17,1,44,4,6,2,1,1,6,
        5,4,2,10,1,6,9,2,8,1,24,1,2,13,7,8,8,2,1,4,1,3,1,3,3,5,2,5,10,9,4,9,12,2,1,6,1,10,1,1,7,7,4,10,8,3,1,13,4,3,1,6,1,3,5,2,1,2,17,16,5,2,16,6,
        1,4,2,1,3,3,6,8,5,11,11,1,3,3,2,4,6,10,9,5,7,4,7,4,7,1,1,4,2,1,3,6,8,7,1,6,11,5,5,3,24,9,4,2,7,13,5,1,8,82,16,61,1,1,1,4,2,2,16,10,3,8,1,1,
        6,4,2,1,3,1,1,1,4,3,8,4,2,2,1,1,1,1,1,6,3,5,1,1,4,6,9,2,1,1,1,2,1,7,2,1,6,1,5,4,4,3,1,8,1,3,3,1,3,2,2,2,2,3,1,6,1,2,1,2,1,3,7,1,8,2,1,2,1,5,
        2,5,3,5,10,1,2,1,1,3,2,5,11,3,9,3,5,1,1,5,9,1,2,1,5,7,9,9,8,1,3,3,3,6,8,2,3,2,1,1,32,6,1,2,15,9,3,7,13,1,3,10,13,2,14,1,13,10,2,1,3,10,4,15,
        2,15,15,10,1,3,9,6,9,32,25,26,47,7,3,2,3,1,6,3,4,3,2,8,5,4,1,9,4,2,2,19,10,6,2,3,8,1,2,2,4,2,1,9,4,4,4,6,4,8,9,2,3,1,1,1,1,3,5,5,1,3,8,4,6,
        2,1,4,12,1,5,3,7,13,2,5,8,1,6,1,2,5,14,6,1,5,2,4,8,15,5,1,23,6,62,2,10,1,1,8,1,2,2,10,4,2,2,9,2,1,1,3,2,3,1,5,3,3,2,1,3,8,1,1,1,11,3,1,1,4,
        3,7,1,14,1,2,3,12,5,2,5,1,6,7,5,7,14,11,1,3,1,8,9,12,2,1,11,8,4,4,2,6,10,9,13,1,1,3,1,5,1,3,2,4,4,1,18,2,3,14,11,4,29,4,2,7,1,3,13,9,2,2,5,
        3,5,20,7,16,8,5,72,34,6,4,22,12,12,28,45,36,9,7,39,9,191,1,1,1,4,11,8,4,9,2,3,22,1,1,1,1,4,17,1,7,7,1,11,31,10,2,4,8,2,3,2,1,4,2,16,4,32,2,
        3,19,13,4,9,1,5,2,14,8,1,1,3,6,19,6,5,1,16,6,2,10,8,5,1,2,3,1,5,5,1,11,6,6,1,3,3,2,6,3,8,1,1,4,10,7,5,7,7,5,8,9,2,1,3,4,1,1,3,1,3,3,2,6,16,
        1,4,6,3,1,10,6,1,3,15,2,9,2,10,25,13,9,16,6,2,2,10,11,4,3,9,1,2,6,6,5,4,30,40,1,10,7,12,14,33,6,3,6,7,3,1,3,1,11,14,4,9,5,12,11,49,18,51,31,
        140,31,2,2,1,5,1,8,1,10,1,4,4,3,24,1,10,1,3,6,6,16,3,4,5,2,1,4,2,57,10,6,22,2,22,3,7,22,6,10,11,36,18,16,33,36,2,5,5,1,1,1,4,10,1,4,13,2,7,
        5,2,9,3,4,1,7,43,3,7,3,9,14,7,9,1,11,1,1,3,7,4,18,13,1,14,1,3,6,10,73,2,2,30,6,1,11,18,19,13,22,3,46,42,37,89,7,3,16,34,2,2,3,9,1,7,1,1,1,2,
        2,4,10,7,3,10,3,9,5,28,9,2,6,13,7,3,1,3,10,2,7,2,11,3,6,21,54,85,2,1,4,2,2,1,39,3,21,2,2,5,1,1,1,4,1,1,3,4,15,1,3,2,4,4,2,3,8,2,20,1,8,7,13,
        4,1,26,6,2,9,34,4,21,52,10,4,4,1,5,12,2,11,1,7,2,30,12,44,2,30,1,1,3,6,16,9,17,39,82,2,2,24,7,1,7,3,16,9,14,44,2,1,2,1,2,3,5,2,4,1,6,7,5,3,
        2,6,1,11,5,11,2,1,18,19,8,1,3,24,29,2,1,3,5,2,2,1,13,6,5,1,46,11,3,5,1,1,5,8,2,10,6,12,6,3,7,11,2,4,16,13,2,5,1,1,2,2,5,2,28,5,2,23,10,8,4,
        4,22,39,95,38,8,14,9,5,1,13,5,4,3,13,12,11,1,9,1,27,37,2,5,4,4,63,211,95,2,2,2,1,3,5,2,1,1,2,2,1,1,1,3,2,4,1,2,1,1,5,2,2,1,1,2,3,1,3,1,1,1,
        3,1,4,2,1,3,6,1,1,3,7,15,5,3,2,5,3,9,11,4,2,22,1,6,3,8,7,1,4,28,4,16,3,3,25,4,4,27,27,1,4,1,2,2,7,1,3,5,2,28,8,2,14,1,8,6,16,25,3,3,3,14,3,
        3,1,1,2,1,4,6,3,8,4,1,1,1,2,3,6,10,6,2,3,18,3,2,5,5,4,3,1,5,2,5,4,23,7,6,12,6,4,17,11,9,5,1,1,10,5,12,1,1,11,26,33,7,3,6,1,17,7,1,5,12,1,11,
        2,4,1,8,14,17,23,1,2,1,7,8,16,11,9,6,5,2,6,4,16,2,8,14,1,11,8,9,1,1,1,9,25,4,11,19,7,2,15,2,12,8,52,7,5,19,2,16,4,36,8,1,16,8,24,26,4,6,2,9,
        5,4,36,3,28,12,25,15,37,27,17,12,59,38,5,32,127,1,2,9,17,14,4,1,2,1,1,8,11,50,4,14,2,19,16,4,17,5,4,5,26,12,45,2,23,45,104,30,12,8,3,10,2,2,
        3,3,1,4,20,7,2,9,6,15,2,20,1,3,16,4,11,15,6,134,2,5,59,1,2,2,2,1,9,17,3,26,137,10,211,59,1,2,4,1,4,1,1,1,2,6,2,3,1,1,2,3,2,3,1,3,4,4,2,3,3,
        1,4,3,1,7,2,2,3,1,2,1,3,3,3,2,2,3,2,1,3,14,6,1,3,2,9,6,15,27,9,34,145,1,1,2,1,1,1,1,2,1,1,1,1,2,2,2,3,1,2,1,1,1,2,3,5,8,3,5,2,4,1,3,2,2,2,12,
        4,1,1,1,10,4,5,1,20,4,16,1,15,9,5,12,2,9,2,5,4,2,26,19,7,1,26,4,30,12,15,42,1,6,8,172,1,1,4,2,1,1,11,2,2,4,2,1,2,1,10,8,1,2,1,4,5,1,2,5,1,8,
        4,1,3,4,2,1,6,2,1,3,4,1,2,1,1,1,1,12,5,7,2,4,3,1,1,1,3,3,6,1,2,2,3,3,3,2,1,2,12,14,11,6,6,4,12,2,8,1,7,10,1,35,7,4,13,15,4,3,23,21,28,52,5,
        26,5,6,1,7,10,2,7,53,3,2,1,1,1,2,163,532,1,10,11,1,3,3,4,8,2,8,6,2,2,23,22,4,2,2,4,2,1,3,1,3,3,5,9,8,2,1,2,8,1,10,2,12,21,20,15,105,2,3,1,1,
        3,2,3,1,1,2,5,1,4,15,11,19,1,1,1,1,5,4,5,1,1,2,5,3,5,12,1,2,5,1,11,1,1,15,9,1,4,5,3,26,8,2,1,3,1,1,15,19,2,12,1,2,5,2,7,2,19,2,20,6,26,7,5,
        2,2,7,34,21,13,70,2,128,1,1,2,1,1,2,1,1,3,2,2,2,15,1,4,1,3,4,42,10,6,1,49,85,8,1,2,1,1,4,4,2,3,6,1,5,7,4,3,211,4,1,2,1,2,5,1,2,4,2,2,6,5,6,
        10,3,4,48,100,6,2,16,296,5,27,387,2,2,3,7,16,8,5,38,15,39,21,9,10,3,7,59,13,27,21,47,5,21,6
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesJapanese()
{
    // 1946 common ideograms code points for Japanese
    // Sourced from http://theinstructionlimit.com/common-kanji-character-ranges-for-xna-spritefont-rendering
    // FIXME: Source a list of the revised 2136 Joyo Kanji list from 2010 and rebuild this.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,6,2,2,1,8,5,7,11,1,2,10,10,8,2,4,20,2,11,8,2,1,2,1,6,2,1,7,5,3,7,1,1,13,7,9,1,4,6,1,2,1,10,1,1,9,2,2,4,5,6,14,1,1,9,3,18,
        5,4,2,2,10,7,1,1,1,3,2,4,3,23,2,10,12,2,14,2,4,13,1,6,10,3,1,7,13,6,4,13,5,2,3,17,2,2,5,7,6,4,1,7,14,16,6,13,9,15,1,1,7,16,4,7,1,19,9,2,7,15,
        2,6,5,13,25,4,14,13,11,25,1,1,1,2,1,2,2,3,10,11,3,3,1,1,4,4,2,1,4,9,1,4,3,5,5,2,7,12,11,15,7,16,4,5,16,2,1,1,6,3,3,1,1,2,7,6,6,7,1,4,7,6,1,1,
        2,1,12,3,3,9,5,8,1,11,1,2,3,18,20,4,1,3,6,1,7,3,5,5,7,2,2,12,3,1,4,2,3,2,3,11,8,7,4,17,1,9,25,1,1,4,2,2,4,1,2,7,1,1,1,3,1,2,6,16,1,2,1,1,3,12,
        20,2,5,20,8,7,6,2,1,1,1,1,6,2,1,2,10,1,1,6,1,3,1,2,1,4,1,12,4,1,3,1,1,1,1,1,10,4,7,5,13,1,15,1,1,30,11,9,1,15,38,14,1,32,17,20,1,9,31,2,21,9,
        4,49,22,2,1,13,1,11,45,35,43,55,12,19,83,1,3,2,3,13,2,1,7,3,18,3,13,8,1,8,18,5,3,7,25,24,9,24,40,3,17,24,2,1,6,2,3,16,15,6,7,3,12,1,9,7,3,3,
        3,15,21,5,16,4,5,12,11,11,3,6,3,2,31,3,2,1,1,23,6,6,1,4,2,6,5,2,1,1,3,3,22,2,6,2,3,17,3,2,4,5,1,9,5,1,1,6,15,12,3,17,2,14,2,8,1,23,16,4,2,23,
        8,15,23,20,12,25,19,47,11,21,65,46,4,3,1,5,6,1,2,5,26,2,1,1,3,11,1,1,1,2,1,2,3,1,1,10,2,3,1,1,1,3,6,3,2,2,6,6,9,2,2,2,6,2,5,10,2,4,1,2,1,2,2,
        3,1,1,3,1,2,9,23,9,2,1,1,1,1,5,3,2,1,10,9,6,1,10,2,31,25,3,7,5,40,1,15,6,17,7,27,180,1,3,2,2,1,1,1,6,3,10,7,1,3,6,17,8,6,2,2,1,3,5,5,8,16,14,
        15,1,1,4,1,2,1,1,1,3,2,7,5,6,2,5,10,1,4,2,9,1,1,11,6,1,44,1,3,7,9,5,1,3,1,1,10,7,1,10,4,2,7,21,15,7,2,5,1,8,3,4,1,3,1,6,1,4,2,1,4,10,8,1,4,5,
        1,5,10,2,7,1,10,1,1,3,4,11,10,29,4,7,3,5,2,3,33,5,2,19,3,1,4,2,6,31,11,1,3,3,3,1,8,10,9,12,11,12,8,3,14,8,6,11,1,4,41,3,1,2,7,13,1,5,6,2,6,12,
        12,22,5,9,4,8,9,9,34,6,24,1,1,20,9,9,3,4,1,7,2,2,2,6,2,28,5,3,6,1,4,6,7,4,2,1,4,2,13,6,4,4,3,1,8,8,3,2,1,5,1,2,2,3,1,11,11,7,3,6,10,8,6,16,16,
        22,7,12,6,21,5,4,6,6,3,6,1,3,2,1,2,8,29,1,10,1,6,13,6,6,19,31,1,13,4,4,22,17,26,33,10,4,15,12,25,6,67,10,2,3,1,6,10,2,6,2,9,1,9,4,4,1,2,16,2,
        5,9,2,3,8,1,8,3,9,4,8,6,4,8,11,3,2,1,1,3,26,1,7,5,1,11,1,5,3,5,2,13,6,39,5,1,5,2,11,6,10,5,1,15,5,3,6,19,21,22,2,4,1,6,1,8,1,4,8,2,4,2,2,9,2,
        1,1,1,4,3,6,3,12,7,1,14,2,4,10,2,13,1,17,7,3,2,1,3,2,13,7,14,12,3,1,29,2,8,9,15,14,9,14,1,3,1,6,5,9,11,3,38,43,20,7,7,8,5,15,12,19,15,81,8,7,
        1,5,73,13,37,28,8,8,1,15,18,20,165,28,1,6,11,8,4,14,7,15,1,3,3,6,4,1,7,14,1,1,11,30,1,5,1,4,14,1,4,2,7,52,2,6,29,3,1,9,1,21,3,5,1,26,3,11,14,
        11,1,17,5,1,2,1,3,2,8,1,2,9,12,1,1,2,3,8,3,24,12,7,7,5,17,3,3,3,1,23,10,4,4,6,3,1,16,17,22,3,10,21,16,16,6,4,10,2,1,1,2,8,8,6,5,3,3,3,39,25,
        15,1,1,16,6,7,25,15,6,6,12,1,22,13,1,4,9,5,12,2,9,1,12,28,8,3,5,10,22,60,1,2,40,4,61,63,4,1,13,12,1,4,31,12,1,14,89,5,16,6,29,14,2,5,49,18,18,
        5,29,33,47,1,17,1,19,12,2,9,7,39,12,3,7,12,39,3,1,46,4,12,3,8,9,5,31,15,18,3,2,2,66,19,13,17,5,3,46,124,13,57,34,2,5,4,5,8,1,1,1,4,3,1,17,5,
        3,5,3,1,8,5,6,3,27,3,26,7,12,7,2,17,3,7,18,78,16,4,36,1,2,1,6,2,1,39,17,7,4,13,4,4,4,1,10,4,2,4,6,3,10,1,19,1,26,2,4,33,2,73,47,7,3,8,2,4,15,
        18,1,29,2,41,14,1,21,16,41,7,39,25,13,44,2,2,10,1,13,7,1,7,3,5,20,4,8,2,49,1,10,6,1,6,7,10,7,11,16,3,12,20,4,10,3,1,2,11,2,28,9,2,4,7,2,15,1,
        27,1,28,17,4,5,10,7,3,24,10,11,6,26,3,2,7,2,2,49,16,10,16,15,4,5,27,61,30,14,38,22,2,7,5,1,3,12,23,24,17,17,3,3,2,4,1,6,2,7,5,1,1,5,1,1,9,4,
        1,3,6,1,8,2,8,4,14,3,5,11,4,1,3,32,1,19,4,1,13,11,5,2,1,8,6,8,1,6,5,13,3,23,11,5,3,16,3,9,10,1,24,3,198,52,4,2,2,5,14,5,4,22,5,20,4,11,6,41,
        1,5,2,2,11,5,2,28,35,8,22,3,18,3,10,7,5,3,4,1,5,3,8,9,3,6,2,16,22,4,5,5,3,3,18,23,2,6,23,5,27,8,1,33,2,12,43,16,5,2,3,6,1,20,4,2,9,7,1,11,2,
        10,3,14,31,9,3,25,18,20,2,5,5,26,14,1,11,17,12,40,19,9,6,31,83,2,7,9,19,78,12,14,21,76,12,113,79,34,4,1,1,61,18,85,10,2,2,13,31,11,50,6,33,159,
        179,6,6,7,4,4,2,4,2,5,8,7,20,32,22,1,3,10,6,7,28,5,10,9,2,77,19,13,2,5,1,4,4,7,4,13,3,9,31,17,3,26,2,6,6,5,4,1,7,11,3,4,2,1,6,2,20,4,1,9,2,6,
        3,7,1,1,1,20,2,3,1,6,2,3,6,2,4,8,1,5,13,8,4,11,23,1,10,6,2,1,3,21,2,2,4,24,31,4,10,10,2,5,192,15,4,16,7,9,51,1,2,1,1,5,1,1,2,1,3,5,3,1,3,4,1,
        3,1,3,3,9,8,1,2,2,2,4,4,18,12,92,2,10,4,3,14,5,25,16,42,4,14,4,2,21,5,126,30,31,2,1,5,13,3,22,5,6,6,20,12,1,14,12,87,3,19,1,8,2,9,9,3,3,23,2,
        3,7,6,3,1,2,3,9,1,3,1,6,3,2,1,3,11,3,1,6,10,3,2,3,1,2,1,5,1,1,11,3,6,4,1,7,2,1,2,5,5,34,4,14,18,4,19,7,5,8,2,6,79,1,5,2,14,8,2,9,2,1,36,28,16,
        4,1,1,1,2,12,6,42,39,16,23,7,15,15,3,2,12,7,21,64,6,9,28,8,12,3,3,41,59,24,51,55,57,294,9,9,2,6,2,15,1,2,13,38,90,9,9,9,3,11,7,1,1,1,5,6,3,2,
        1,2,2,3,8,1,4,4,1,5,7,1,4,3,20,4,9,1,1,1,5,5,17,1,5,2,6,2,4,1,4,5,7,3,18,11,11,32,7,5,4,7,11,127,8,4,3,3,1,10,1,1,6,21,14,1,16,1,7,1,3,6,9,65,
        51,4,3,13,3,10,1,1,12,9,21,110,3,19,24,1,1,10,62,4,1,29,42,78,28,20,18,82,6,3,15,6,84,58,253,15,155,264,15,21,9,14,7,58,40,39,
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00)*2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesCyrillic()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesThai()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x2010, 0x205E, // Punctuations
        0x0E00, 0x0E7F, // Thai
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesVietnamese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x0102, 0x0103,
        0x0110, 0x0111,
        0x0128, 0x0129,
        0x0168, 0x0169,
        0x01A0, 0x01A1,
        0x01AF, 0x01B0,
        0x1EA0, 0x1EF9,
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void ImFontGlyphRangesBuilder::AddText(const char* text, const char* text_end)
{
    while (text_end ? (text < text_end) : *text)
    {
        unsigned int c = 0;
        int c_len = ImTextCharFromUtf8(&c, text, text_end);
        text += c_len;
        if (c_len == 0)
            break;
        AddChar((ImWchar)c);
    }
}

void ImFontGlyphRangesBuilder::AddRanges(const ImWchar* ranges)
{
    for (; ranges[0]; ranges += 2)
        for (ImWchar c = ranges[0]; c <= ranges[1]; c++)
            AddChar(c);
}

void ImFontGlyphRangesBuilder::BuildRanges(ImVector<ImWchar>* out_ranges)
{
    const int max_codepoint = IM_UNICODE_CODEPOINT_MAX;
    for (int n = 0; n <= max_codepoint; n++)
        if (GetBit(n))
        {
            out_ranges->push_back((ImWchar)n);
            while (n < max_codepoint && GetBit(n + 1))
                n++;
            out_ranges->push_back((ImWchar)n);
        }
    out_ranges->push_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    FallbackChar = (ImWchar)'?';
    EllipsisChar = (ImWchar)-1;
    DisplayOffset = ImVec2(0.0f, 0.0f);
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    ConfigData = NULL;
    ConfigDataCount = 0;
    DirtyLookupTables = false;
    Scale = 1.0f;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
    memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
}

ImFont::~ImFont()
{
    ClearOutputData();
}

void    ImFont::ClearOutputData()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    Glyphs.clear();
    IndexAdvanceX.clear();
    IndexLookup.clear();
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    DirtyLookupTables = true;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    // Build lookup table
    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexAdvanceX.clear();
    IndexLookup.clear();
    DirtyLookupTables = false;
    memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
    GrowIndex(max_codepoint + 1);
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
        IndexLookup[codepoint] = (ImWchar)i;

        // Mark 4K page as used
        const int page_n = codepoint / 4096;
        Used4kPagesMap[page_n >> 3] |= 1 << (page_n & 7);
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if (FindGlyph((ImWchar)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times (FIXME: Flaky)
            Glyphs.resize(Glyphs.Size + 1);
        ImFontGlyph& tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((ImWchar)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.AdvanceX *= IM_TABSIZE;
        IndexAdvanceX[(int)tab_glyph.Codepoint] = (float)tab_glyph.AdvanceX;
        IndexLookup[(int)tab_glyph.Codepoint] = (ImWchar)(Glyphs.Size-1);
    }

    // Mark special glyphs as not visible (note that AddGlyph already mark as non-visible glyphs with zero-size polygons)
    SetGlyphVisible((ImWchar)' ', false);
    SetGlyphVisible((ImWchar)'\t', false);

    // Setup fall-backs
    FallbackGlyph = FindGlyphNoFallback(FallbackChar);
    FallbackAdvanceX = FallbackGlyph ? FallbackGlyph->AdvanceX : 0.0f;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexAdvanceX[i] < 0.0f)
            IndexAdvanceX[i] = FallbackAdvanceX;
}

// API is designed this way to avoid exposing the 4K page size
// e.g. use with IsGlyphRangeUnused(0, 255)
bool ImFont::IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last)
{
    unsigned int page_begin = (c_begin / 4096);
    unsigned int page_last = (c_last / 4096);
    for (unsigned int page_n = page_begin; page_n <= page_last; page_n++)
        if ((page_n >> 3) < sizeof(Used4kPagesMap))
            if (Used4kPagesMap[page_n >> 3] & (1 << (page_n & 7)))
                return false;
    return true;
}

void ImFont::SetGlyphVisible(ImWchar c, bool visible)
{
    if (ImFontGlyph* glyph = (ImFontGlyph*)(void*)FindGlyph((ImWchar)c))
        glyph->Visible = visible ? 1 : 0;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

void ImFont::GrowIndex(int new_size)
{
    IM_ASSERT(IndexAdvanceX.Size == IndexLookup.Size);
    if (new_size <= IndexLookup.Size)
        return;
    IndexAdvanceX.resize(new_size, -1.0f);
    IndexLookup.resize(new_size, (ImWchar)-1);
}

// x0/y0/x1/y1 are offset from the character upper-left layout position, in pixels. Therefore x0/y0 are often fairly close to zero.
// Not to be mistaken with texture coordinates, which are held by u0/v0/u1/v1 in normalized format (0.0..1.0 on each texture axis).
void ImFont::AddGlyph(ImWchar codepoint, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x)
{
    Glyphs.resize(Glyphs.Size + 1);
    ImFontGlyph& glyph = Glyphs.back();
    glyph.Codepoint = (unsigned int)codepoint;
    glyph.Visible = (x0 != x1) && (y0 != y1);
    glyph.X0 = x0;
    glyph.Y0 = y0;
    glyph.X1 = x1;
    glyph.Y1 = y1;
    glyph.U0 = u0;
    glyph.V0 = v0;
    glyph.U1 = u1;
    glyph.V1 = v1;
    glyph.AdvanceX = advance_x + ConfigData->GlyphExtraSpacing.x;  // Bake spacing into AdvanceX

    if (ConfigData->PixelSnapH)
        glyph.AdvanceX = IM_ROUND(glyph.AdvanceX);

    // Compute rough surface usage metrics (+1 to account for average padding, +0.99 to round)
    DirtyLookupTables = true;
    MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * ContainerAtlas->TexWidth + 1.99f) * (int)((glyph.V1 - glyph.V0) * ContainerAtlas->TexHeight + 1.99f);
}

void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
    IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
    unsigned int index_size = (unsigned int)IndexLookup.Size;

    if (dst < index_size && IndexLookup.Data[dst] == (ImWchar)-1 && !overwrite_dst) // 'dst' already exists
        return;
    if (src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
        return;

    GrowIndex(dst + 1);
    IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (ImWchar)-1;
    IndexAdvanceX[dst] = (src < index_size) ? IndexAdvanceX.Data[src] : 1.0f;
}

const ImFontGlyph* ImFont::FindGlyph(ImWchar c) const
{
    if (c >= (size_t)IndexLookup.Size)
        return FallbackGlyph;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return FallbackGlyph;
    return &Glyphs.Data[i];
}

const ImFontGlyph* ImFont::FindGlyphNoFallback(ImWchar c) const
{
    if (c >= (size_t)IndexLookup.Size)
        return NULL;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return NULL;
    return &Glyphs.Data[i];
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured. Please submit failing cases!
    // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)*s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
        if (c == 0)
            break;

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX);
        if (ImCharIsBlankW(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
                word_end = s;
            }
            blank_width += char_width;
            inside_word = false;
        }
        else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            }
            else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width > wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    return s;
}

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    const float line_height = size;
    const float scale = size / FontSize;

    ImVec2 text_size = ImVec2(0,0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        const char* prev_s = s;
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
            if (c == '\r')
                continue;
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX) * scale;
        if (line_width + char_width >= max_width)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

void ImFont::RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const
{
    const ImFontGlyph* glyph = FindGlyph(c);
    if (!glyph || !glyph->Visible)
        return;
    float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
    pos.x = IM_FLOOR(pos.x + DisplayOffset.x);
    pos.y = IM_FLOOR(pos.y + DisplayOffset.y);
    draw_list->PrimReserve(6, 4);
    draw_list->PrimRectUV(ImVec2(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale), ImVec2(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale), ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V1), col);
}

void ImFont::RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // ImGui:: functions generally already provides a valid text_end, so this is merely to handle direct calls.

    // Align to be pixel perfect
    pos.x = IM_FLOOR(pos.x + DisplayOffset.x);
    pos.y = IM_FLOOR(pos.y + DisplayOffset.y);
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    // Fast-forward to first visible line
    const char* s = text_begin;
    if (y + line_height < clip_rect.y && !word_wrap_enabled)
        while (y + line_height < clip_rect.y && s < text_end)
        {
            s = (const char*)memchr(s, '\n', text_end - s);
            s = s ? s + 1 : text_end;
            y += line_height;
        }

    // For large text, scan for the last visible line in order to avoid over-reserving in the call to PrimReserve()
    // Note that very large horizontal line will still be affected by the issue (e.g. a one megabyte string buffer without a newline will likely crash atm)
    if (text_end - s > 10000 && !word_wrap_enabled)
    {
        const char* s_end = s;
        float y_end = y;
        while (y_end < clip_rect.w && s_end < text_end)
        {
            s_end = (const char*)memchr(s_end, '\n', text_end - s_end);
            s_end = s_end ? s_end + 1 : text_end;
            y_end += line_height;
        }
        text_end = s_end;
    }
    if (s == text_end)
        return;

    // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
    const int vtx_count_max = (int)(text_end - s) * 4;
    const int idx_count_max = (int)(text_end - s) * 6;
    const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
    draw_list->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;
                if (y > clip_rect.w)
                    break; // break out of main loop
                continue;
            }
            if (c == '\r')
                continue;
        }

        const ImFontGlyph* glyph = FindGlyph((ImWchar)c);
        if (glyph == NULL)
            continue;

        float char_width = glyph->AdvanceX * scale;
        if (glyph->Visible)
        {
            // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
            float x1 = x + glyph->X0 * scale;
            float x2 = x + glyph->X1 * scale;
            float y1 = y + glyph->Y0 * scale;
            float y2 = y + glyph->Y1 * scale;
            if (x1 <= clip_rect.z && x2 >= clip_rect.x)
            {
                // Render a character
                float u1 = glyph->U0;
                float v1 = glyph->V0;
                float u2 = glyph->U1;
                float v2 = glyph->V1;

                // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                if (cpu_fine_clip)
                {
                    if (x1 < clip_rect.x)
                    {
                        u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                        x1 = clip_rect.x;
                    }
                    if (y1 < clip_rect.y)
                    {
                        v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                        y1 = clip_rect.y;
                    }
                    if (x2 > clip_rect.z)
                    {
                        u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                        x2 = clip_rect.z;
                    }
                    if (y2 > clip_rect.w)
                    {
                        v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                        y2 = clip_rect.w;
                    }
                    if (y1 >= y2)
                    {
                        x += char_width;
                        continue;
                    }
                }

                // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                {
                    idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx+1); idx_write[2] = (ImDrawIdx)(vtx_current_idx+2);
                    idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx+2); idx_write[5] = (ImDrawIdx)(vtx_current_idx+3);
                    vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                    vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                    vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                    vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                    vtx_write += 4;
                    vtx_current_idx += 4;
                    idx_write += 6;
                }
            }
        }
        x += char_width;
    }

    // Give back unused vertices (clipped ones, blanks) ~ this is essentially a PrimUnreserve() action.
    draw_list->VtxBuffer.Size = (int)(vtx_write - draw_list->VtxBuffer.Data); // Same as calling shrink()
    draw_list->IdxBuffer.Size = (int)(idx_write - draw_list->IdxBuffer.Data);
    draw_list->CmdBuffer[draw_list->CmdBuffer.Size-1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_IdxWritePtr = idx_write;
    draw_list->_VtxCurrentIdx = vtx_current_idx;
}

//-----------------------------------------------------------------------------
// [SECTION] ImGui Internal Render Helpers
//-----------------------------------------------------------------------------
// Vaguely redesigned to stop accessing ImGui global state:
// - RenderArrow()
// - RenderBullet()
// - RenderCheckMark()
// - RenderMouseCursor()
// - RenderArrowPointingAt()
// - RenderRectFilledRangeH()
//-----------------------------------------------------------------------------
// Function in need of a redesign (legacy mess)
// - RenderColorRectWithAlphaCheckerboard()
//-----------------------------------------------------------------------------

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would be positioned). To e.g. denote expanded/collapsed state
void ImGui::RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 a, b, c;
    switch (dir)
    {
    case ImGuiDir_Up:
    case ImGuiDir_Down:
        if (dir == ImGuiDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.750f) * r;
        b = ImVec2(-0.866f, -0.750f) * r;
        c = ImVec2(+0.866f, -0.750f) * r;
        break;
    case ImGuiDir_Left:
    case ImGuiDir_Right:
        if (dir == ImGuiDir_Left) r = -r;
        a = ImVec2(+0.750f, +0.000f) * r;
        b = ImVec2(-0.750f, +0.866f) * r;
        c = ImVec2(-0.750f, -0.866f) * r;
        break;
    case ImGuiDir_None:
    case ImGuiDir_COUNT:
        IM_ASSERT(0);
        break;
    }
    draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
}

void ImGui::RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col)
{
    draw_list->AddCircleFilled(pos, draw_list->_Data->FontSize * 0.20f, col, 8);
}

void ImGui::RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz)
{
    float thickness = ImMax(sz / 5.0f, 1.0f);
    sz -= thickness * 0.5f;
    pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

    float third = sz / 3.0f;
    float bx = pos.x + third;
    float by = pos.y + sz - third * 0.5f;
    draw_list->PathLineTo(ImVec2(bx - third, by - third));
    draw_list->PathLineTo(ImVec2(bx, by));
    draw_list->PathLineTo(ImVec2(bx + third * 2.0f, by - third * 2.0f));
    draw_list->PathStroke(col, false, thickness);
}

void ImGui::RenderMouseCursor(ImDrawList* draw_list, ImVec2 pos, float scale, ImGuiMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow)
{
    if (mouse_cursor == ImGuiMouseCursor_None)
        return;
    IM_ASSERT(mouse_cursor > ImGuiMouseCursor_None && mouse_cursor < ImGuiMouseCursor_COUNT);

    ImFontAtlas* font_atlas = draw_list->_Data->Font->ContainerAtlas;
    ImVec2 offset, size, uv[4];
    if (font_atlas->GetMouseCursorTexData(mouse_cursor, &offset, &size, &uv[0], &uv[2]))
    {
        pos -= offset;
        const ImTextureID tex_id = font_atlas->TexID;
        draw_list->PushTextureID(tex_id);
        draw_list->AddImage(tex_id, pos + ImVec2(1,0)*scale, pos + ImVec2(1,0)*scale + size*scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos + ImVec2(2,0)*scale, pos + ImVec2(2,0)*scale + size*scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos,                     pos + size*scale,                     uv[2], uv[3], col_border);
        draw_list->AddImage(tex_id, pos,                     pos + size*scale,                     uv[0], uv[1], col_fill);
        draw_list->PopTextureID();
    }
}

// Render an arrow. 'pos' is position of the arrow tip. half_sz.x is length from base to tip. half_sz.y is length on each side.
void ImGui::RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, ImGuiDir direction, ImU32 col)
{
    switch (direction)
    {
    case ImGuiDir_Left:  draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), pos, col); return;
    case ImGuiDir_Right: draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), pos, col); return;
    case ImGuiDir_Up:    draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), pos, col); return;
    case ImGuiDir_Down:  draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), pos, col); return;
    case ImGuiDir_None: case ImGuiDir_COUNT: break; // Fix warnings
    }
}

static inline float ImAcos01(float x)
{
    if (x <= 0.0f) return IM_PI * 0.5f;
    if (x >= 1.0f) return 0.0f;
    return ImAcos(x);
    //return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f; // Cheap approximation, may be enough for what we do.
}

// FIXME: Cleanup and move code to ImDrawList.
void ImGui::RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding)
{
    if (x_end_norm == x_start_norm)
        return;
    if (x_start_norm > x_end_norm)
        ImSwap(x_start_norm, x_end_norm);

    ImVec2 p0 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_start_norm), rect.Min.y);
    ImVec2 p1 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_end_norm), rect.Max.y);
    if (rounding == 0.0f)
    {
        draw_list->AddRectFilled(p0, p1, col, 0.0f);
        return;
    }

    rounding = ImClamp(ImMin((rect.Max.x - rect.Min.x) * 0.5f, (rect.Max.y - rect.Min.y) * 0.5f) - 1.0f, 0.0f, rounding);
    const float inv_rounding = 1.0f / rounding;
    const float arc0_b = ImAcos01(1.0f - (p0.x - rect.Min.x) * inv_rounding);
    const float arc0_e = ImAcos01(1.0f - (p1.x - rect.Min.x) * inv_rounding);
    const float half_pi = IM_PI * 0.5f; // We will == compare to this because we know this is the exact value ImAcos01 can return.
    const float x0 = ImMax(p0.x, rect.Min.x + rounding);
    if (arc0_b == arc0_e)
    {
        draw_list->PathLineTo(ImVec2(x0, p1.y));
        draw_list->PathLineTo(ImVec2(x0, p0.y));
    }
    else if (arc0_b == 0.0f && arc0_e == half_pi)
    {
        draw_list->PathArcToFast(ImVec2(x0, p1.y - rounding), rounding, 3, 6); // BL
        draw_list->PathArcToFast(ImVec2(x0, p0.y + rounding), rounding, 6, 9); // TR
    }
    else
    {
        draw_list->PathArcTo(ImVec2(x0, p1.y - rounding), rounding, IM_PI - arc0_e, IM_PI - arc0_b, 3); // BL
        draw_list->PathArcTo(ImVec2(x0, p0.y + rounding), rounding, IM_PI + arc0_b, IM_PI + arc0_e, 3); // TR
    }
    if (p1.x > rect.Min.x + rounding)
    {
        const float arc1_b = ImAcos01(1.0f - (rect.Max.x - p1.x) * inv_rounding);
        const float arc1_e = ImAcos01(1.0f - (rect.Max.x - p0.x) * inv_rounding);
        const float x1 = ImMin(p1.x, rect.Max.x - rounding);
        if (arc1_b == arc1_e)
        {
            draw_list->PathLineTo(ImVec2(x1, p0.y));
            draw_list->PathLineTo(ImVec2(x1, p1.y));
        }
        else if (arc1_b == 0.0f && arc1_e == half_pi)
        {
            draw_list->PathArcToFast(ImVec2(x1, p0.y + rounding), rounding, 9, 12); // TR
            draw_list->PathArcToFast(ImVec2(x1, p1.y - rounding), rounding, 0, 3);  // BR
        }
        else
        {
            draw_list->PathArcTo(ImVec2(x1, p0.y + rounding), rounding, -arc1_e, -arc1_b, 3); // TR
            draw_list->PathArcTo(ImVec2(x1, p1.y - rounding), rounding, +arc1_b, +arc1_e, 3); // BR
        }
    }
    draw_list->PathFillConvex(col);
}

// Helper for ColorPicker4()
// NB: This is rather brittle and will show artifact when rounding this enabled if rounded corners overlap multiple cells. Caller currently responsible for avoiding that.
// Spent a non reasonable amount of time trying to getting this right for ColorButton with rounding+anti-aliasing+ImGuiColorEditFlags_HalfAlphaPreview flag + various grid sizes and offsets, and eventually gave up... probably more reasonable to disable rounding alltogether.
// FIXME: uses ImGui::GetColorU32
void ImGui::RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 col, float grid_step, ImVec2 grid_off, float rounding, int rounding_corners_flags)
{
    if (((col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) < 0xFF)
    {
        ImU32 col_bg1 = ImGui::GetColorU32(ImAlphaBlendColors(IM_COL32(204, 204, 204, 255), col));
        ImU32 col_bg2 = ImGui::GetColorU32(ImAlphaBlendColors(IM_COL32(128, 128, 128, 255), col));
        draw_list->AddRectFilled(p_min, p_max, col_bg1, rounding, rounding_corners_flags);

        int yi = 0;
        for (float y = p_min.y + grid_off.y; y < p_max.y; y += grid_step, yi++)
        {
            float y1 = ImClamp(y, p_min.y, p_max.y), y2 = ImMin(y + grid_step, p_max.y);
            if (y2 <= y1)
                continue;
            for (float x = p_min.x + grid_off.x + (yi & 1) * grid_step; x < p_max.x; x += grid_step * 2.0f)
            {
                float x1 = ImClamp(x, p_min.x, p_max.x), x2 = ImMin(x + grid_step, p_max.x);
                if (x2 <= x1)
                    continue;
                int rounding_corners_flags_cell = 0;
                if (y1 <= p_min.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopRight; }
                if (y2 >= p_max.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotRight; }
                rounding_corners_flags_cell &= rounding_corners_flags;
                draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col_bg2, rounding_corners_flags_cell ? rounding : 0.0f, rounding_corners_flags_cell);
            }
        }
    }
    else
    {
        draw_list->AddRectFilled(p_min, p_max, col, rounding, rounding_corners_flags);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char *input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier_out_e, *stb__barrier_out_b;
static const unsigned char *stb__barrier_in_b;
static unsigned char *stb__dout;
static void stb__match(const unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e+1; return; }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(const unsigned char *data, unsigned int length)
{
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e+1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char *stb_decompress_token(const unsigned char *i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
        else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen = buflen % 5552;

    unsigned long i;
    while (buflen) {
        for (i=0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, const unsigned char *i, unsigned int /*length*/)
{
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    const unsigned int olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        const unsigned char *old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int) stb__in4(2))
                    return 0;
                return olen;
            } else {
                IM_ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        IM_ASSERT(stb__dout <= output + olen);
        if (stb__dout > output + olen)
            return 0;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string encoding).
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
//-----------------------------------------------------------------------------
static const char tahoma_compressed_data_base85[46390 + 1] =
"7])#######6rND:'/###I),##c'ChLYqH##$@;*>7#xw11IFrdid>11fY;99<k9'#r1U(aoqEn/(RdL<6Co)2rA^01f2NP&]FxF>,)vI_)+m<-MKD21%[n42%<9:vT,>>#CtJw5aNV=B"
"3`Pw;mQUV$P6YY#j1XGHBJTLSO7_c)wU(&$lIk7D+Q+^XYlQS%Nc68%ea(*HUYI%#tT^C-L>U&6+-0%Jm<A=3?e;A+YGB`N8.d<Biv$`C',>>#s1dD4AJr9.VI83'9F=2Cw>eW.;h^`I"
"V5#]7I'aX%IsTSIo6Q<BwYJpJ)i%oSZ6n,a0iR/GV8N87Y*gcN_qp/1+>00F(gQ>G@4[(W%^v%+V$S+HDLGc2sutF`E5YY#xbFVCFpqD2TVfi'a%`1.]nRfL_C*j0TNkdFodPirJ[m##"
"Gd_Pg(FtZNLw0]X;*j2^Ks0<.-MYY#Z0Yh#<H8X%'<V,#N^bxu4rVH&9Ow3+Xx=DkGGm50c&`lA&kj%=8C'L5HAFS7-B&##Z63,;1FUB#C'NAFeXZV$Do>QT[h$##([YGMX'BGVYN`o["
"8=aP&5oqV$1`L;$SSh7[&DY8&/]ah#b.Gd=sks/1$5Rv$FAjD#sKG>#5TG-Mf`;;$/dd&MLigcN(*2t-Rr[fLO3ixXA$YA4nZQMgj:J5Af<-mfHSJ/Cn+9S[JXJ/(k?n-$'97R*UIh--"
"W0a`EXA6Sn]-/F%nP2j'Po<R*rIm-$VOr-$6Cn-$*um-$Fsk-$Eqn-$tOm-$,%n-$Id6T8C:T+rC>>8J(vPYd4cYcabb>1i_`k%FJK-GM@@cCswL6PS8<.58ug/F%Ftn-$aS1PSgm^w'"
"V[3PS*gdo@/vpo%tC6PSep8PSH^`k+:XL>Y(dscD;F5F%FA7R3$>Vc;)1V:vB?HcM<@C%t&`=A+*_WS%ePE(s2Z2;685mDXj&U`3/r[PB$1=gMwhfIqk'lds?uNY5r7F`NiOou5f@*$6"
"C1ku5$&=>,U%?wH,Be.h;Pxo7ukcxtemU&PksH`N9t[:.d05>5rsgcN[fH#6un'F7u:GP8=Aec)bh72LT#4w$7^[Y,'f;5&/@q#$;h2%$$Nn*.?M#lLQH*b.Tvr4#*$iH-;6T;-x5T;-"
"a>pV-UUd--D/B;I((CBuD$KY>6[IYG%7q+Di-RMUBg(_]dvq7#:)m<-.s.>-3nL5/?r@H#Lf;uL;$'-#3)X9#t*m<-SX`=-m7T;-ZVjY.Y<###%U5s-U3$&MTb+5v_iP/$LhY'v%cw8#"
"(o('8:IPF%Epo=#*BT;-S.r5/bg#3#.6$#8s3?Z$FP^)Pq'G9#UKi9.Btfe$/16?$ehOU$N1QD-&j)M-DAU*Ra#u5&h%go[C+]A45:qB#N.>>#s^6K#E.-&M&$RR$N9PpKQJE,#C6^.$"
"t%F9#s)m<-d3S>-Y;iP/)S$s$r7F`N&Fg9;XN.mf]/f29'4DZH[FqrQwc6?.5D12#bj@)v]OD6MY(='Mo<ZwLc-/S%hv=Y#@cqR#c[hpLM(q9$ssQ4$soB+$xA%+MrS3:MQ0>DW&+_AO"
"UHI<-GOJ/($nn-$Chc-66gKZ#joIcMt-gfL'J5s-nlRfLQC?>#G6FGM5A-),Sfr.LO[%/1GR;;$<h/2'LMBuuTVU]=T6FSI6$arQwNI`j.lkJ)FBx],VMQ8/#-s]53&Hv-0on34^:f34"
">Y;X1N0dlAq-<5KemS]cd=.0)B*A&,k7IX1/4SX1KMA(s])+##xp[xF)ch(<MCJ]4iB5.q[0vb_QN_I^K$G1].gx_V#3.(`kRdh--^2_ASt;%$>vViBB88JCFPo+DJiOcD[r&qrEeU.#"
"K/`$#Vmk.#Z#(/#_/:/#c;L/#gG_/#kSq/#o`-0#sl?0#wxQ0#%/e0#);w0#-G31#1SE1#5`W1#9lj1#=x&2#A.92#E:K2#IF^2#MRp2#Q_,3#Uk>3#YwP3#^-d3#b9v3#fE24#jQD4#"
"n^V4#rji4#vv%5#$-85#(9J5#,E]5#0Qo5#4^+6#8j=6#<vO6#@,c6#D8u6#HD17#LPC7#*JY##&P4F%G)###O0uT%j$###'>uu#9@n8%I3TQ&Y&;k'jow-)$c^F*4UD`+DH+#-T;h;."
"e.NT/uw4n0/kq02?^WI3OP>c4`C%&6p6b>7**HW8:s.q9Jfk3;ZXQL<kK8f=%?u(?52[A@E%BZAjH.GMKmqeQCp9XV_l)7Xsq4lY-eq.[@jsG]Tx_#`*GcBhoWnQD)-+RFH/uoDBM:3#"
"PjOfCjG/e%#A-DER9D@_s3].#XKH`EOF6/LlNOUbc?Adg$=W6#/6rCj-d*##k>P)h5)rXLSmCiPh(tcRAMr#UiR9Me`.ZZ%E9'##.iES7*=j`5WV,G4GdE.37q_k1'(#R0m4<9/]AUv-"
"LNo],<[2D+,iK+*rueh(b,)O'Q9B6&AF[s$1SuY#Q5$C#2t5B?vuJj0L#(Z-?]+x'OLkx'$C3p.MScV[;AGc.fcPc.K=@]PS7D]PN]TcNYUJ[[.T,a9Q)(3MX'q5#VVn5#QGRG)hx2XC"
"TQTe;mMZ*N,%:6C`9mjD?8'6CBhPq2mTm2(+Y)6q=NnUMi<Fd&,u-T8A5k31?/ZQ&0hrS&]*Uw0lU8Q#-:6GMO<RM9VgQm#fvbK1=)`k0@%u598qY,EU<K%BG@^;.h)3+MN#b'On;v2v"
"lS2P+Qr*O4Eec&#Ab^KMX&8S[$S&K%Ns?Q/Hu6g7lwD]O7cxdaX^gB(gixs0qW[2C?0R][dMoS1YU%v#J`q->YMJ'J7=fpAa$)N$4'BNMqw$V[[oG59d^3uLF$JY-(4_w'.kamL1[vhL"
"?2@Q8:Lhca.<IhLElMZ[WMH&:OJ&n9K>sx#u;)fDQewQ/5l[I-tceC?dn'Z-`AV#6G#3j*'JHDO90QNDw9m31B4HQ1BaQ.2jD(Z-_Z6H+gI<wAZ(7*.cN16MJI20EjG+YCU2.&PG<Q[0"
"oN;C.(I+6V0icOO5@f59&FL6M7q.B.V]^)>&`b&#YFOPMoP4g7Sr82N5Mp;-)eg40]oG59d;uv8K6DgE>71I=_Z/L,g4_5Bm?=dX7F)=-Hc>hLu5,0NpX>;-D8a;.WCYs/xQfE-)0iC?"
"%8[A-^l:p.KX%AO/<9_8'6KZ?tZX0;4BI44/OEp.V.HDOvT+Q#,&S/p^9.AO<4+3N.XIp.WSn+M;jZ?.4heOOHscP9OJ_@BjM$[BDwn9.KQx3;o&%tA9-<d<Z+YfLYO3qOm%3*PHRDnA"
"IrPP1(1)I09uXH=LZGY-#3q[')np['W3$K%ae)Q8n1dd.-#+N$B/K;-oSp;-2<Ko1gld>-m82>>Kb&V[>+45M%mGoL0V#<-/^Sn.*TI75@Nfo7#^d/;wSDQC)V.UDBtLC&MfAp&E($hO"
")^:MBNR[V[(5klM)jG&#SR#6#m/xZ?ee$6MO%RXNs[A@>ab72L5+<+7KC<Z-1<66#$####mj&6/;T0n'LH&49xW-Q#hvJ>-StK8/-e5B?qn]fLupls-r',,Mob=:&Ler<-RG7l0U8R)0"
"bbr9.=NtV[jMDj$>*+>6i)/L(<E7@#q[Le==sB%@lZCHOMl9dM$^=g0rB3p.IkP?.fR.`8en@pAVN6tRM:Le=bR+H2Tb96C_N6dO+1WALMGJm#WB1&4^k0a96CRDOw9x/sarPDO4q.=-"
"eu4hY5as)>d<RUMKUFY-jg,T%MF;##;m-B#D3?g7Z6W%@E_f$#vZ=1>1SV>#a0>?8*WBjLv/ES_.gES747(^#S/BU)_nX>-,xk&#c&0I=RgwV%m0Ta+s;%##s'c?-@hlHMrc89_g@/:)"
"cMt9)VPSiBwN8:)W8l_/0;s9)5S8:)qW1:)u@q0#GWZ6#SC_6_*=VC?f2DYP6YMuuPcdl8XNto%9g,?AC+i^>*aG59-w-Q#8MI6/`TZ6#GWxiLAwH81=8e2$9WkC?J'JDO&[b6C4M^B-"
"W2k^[-D?F%LKUNCVo8@-*6rC?5d&k$;4;^?I7;9.vV%d5g6jY?M%l63*WQDOaV3B--/ImL7IMX-LwIb.)7Z81Bqc2$CZ8/=FNF$9tvTs8/h'Z-O8&sa=s&saRbqY?DX4?-3SQxL34kpD"
"@hAN-,f]j-Jwe--l$7@9SjQ][/Fg5#kQR:MK[sC?C3WAL3ItCOSdOwB;J=2_-Q?>(*4VuPpmS/Ne8Z6#v-a3=b&u59XU1n<%Qi*%PH/a+4Kr9.V/:Z-s_vY?3%@E.L]Le=W%f,21P2>>"
"T5&,#Jvv1K4b+/(a7jV7YZ?>Q'.$$$.`HVH`$*.$oQV/#hX/E#Fu^N1SDwQ#xh.`+/T5>G5b`]-f9-A#@*,##PAg;-o5T;-96T;-i6T;-37T;-UI5s-fk'hL`B3mL(YVpLHp$tLSeduu"
"@hW=]LJeO_4:=mB*#.IMOX+=]o1nb%gcOjLVxFX]NS+m'=M`,#d7T;-H0RE.en%D+LJLv?08@IMeL,B(blH=.C'66&Ga.&G.d3&G%YCb@l,bv)]ab;.bupe)ZD8FIH%9=L:6%?2q6x:."
"r?^6/9U9jCq8LHM4=FNNf5R?]xpt;.8VbQ0J-uMC@+oiLY=R?]Nv*/Mow-?]aden*CVh/7g`,<-q6q4.%_3IM`+IZ]e5V3X+4g)0tab;.+tt%@7,AT]S#+M2v6=D<RqqG<Re.F%dw`xF"
"K%72L&%_^#3]x.Cd)k-$n-?lfV4l%l%uT^#V#d5GRo$E.CfKI)U>*Q93S*+N`m06&kef88'dLhLl:8S#0NVN'p[Nv64P[IM=D;;]Cgh&4/d,,2k56%-xVZ*-.0+%-:fA>5.5l-$<3;>5"
"f,Ix,U^ru5gJ-/(%#[]4/qR5'``1*4sKDZ)jPfs&1@CB#9DDD3tqSq)Vf-F%6Z,F%.rRC#8b[w,WV&#5B,%#5NE$j'runr#/u3W.B<-O7aQ>X+D`+U#lbf876SqxF;[ZldV_[ldaCpS#"
"Y+-BdIqN%b&9KK('edT#.Q%Z7OND-3<R4N-oAPs<$0mAL/t2g[:bN5lSj8I]xBEX1*:#9^bog#5A)ri'FEjc#'d:H>Gtgb0sQcR#GF[Va+9Jw).#<R#*]Fx,*V=x,)M4x,_[`(-1e?$-"
";E<%-8NE/1k5x%-;w]`7C(8%-kq]G2QkF&-6mG#-7T##-l>[(-`5T;-F]rS%H;X.3bw@4C4)9f3UJA(>ue7`,(h((l*NV%$)nTj]&0rH4qn*i]8l3jL3;jm7SVij'qlh1:-9/J#s+N'f"
"I%+*&kFoDGjMJ3_.Afv%FmJC4PNee)k&kdaJ[W%&v'e=TN?KqL]<MrCgdYwCCK:8[2rtQMdeC3Ckdi)39gLER&6bM#MT[iK3eYi#Ed1##N.aja*sd8B];A/(5lYDQ;7uQ#N@sZ)FTLR)"
"lQS#MH54L)>?*L)Q9wK)e3nK)^,S.0cP7PfWk&K1sO81(DvkS8>_`[,B,-F%oq:=6N1xC=jAPl9_0r=6&ZOw9heFk4_wmq7-iGC+.-k-$*MM@#QF=W8aPu>#cHNh#R-BC#)#1J#TPxP#"
"`hET#k4PY#=Y>>#b>$(#'mk.#RD]5#^[*9#i(5>#xiM^4c9#j'Orap#MClu#6_8m&sgj;$E<D>5XNUf<ej*$MLiP#)d5g*>*_F@#9+Rc=R5w)3Y`_ACv_2LPkDXGSugQ][(l28_PP2Q0"
"3w<S[hw]][PbW1N$bai0Li,%HUv:GHjof2$ew`29NPb/%,H)8M6$;-M2*]9.lQS39rwL_&r5Gp8.v449tBB:.3IcrLkfafLnuP<-?Bpr-gF'5N['AY-T%w:2`YoBJZK$+%<h*v0^.1lL"
"FWq1MF*R#PBZ:GHPItNM7:CSMRo:3_CqSNDJRZHdi(,#HSjcs-)Yqr$a0bX$DFTQ$Y'o)$[0`$#aE)8v&Iqfu8XR)$iRWjLY%%3#2,c6#;CH&M-J,s-'cg[Ms^V/#AVL7#[g:EN(Wf0#"
"_d`4#vH9:#)6FR/Z.IfU*a3S[miSSSR:r5K^j^lJM[kq)Fbu=Y0/0P]L99]bcAEk#7XH>,[<s`bknOcMdwn;#YuWT#&+02#a<#s-*JMmLM+AjL+pO1#`jPlLamr8[UeHuu[KH87Nu<Se"
"vd[]Fw%=F7CLKsL@:J4$Kti4#mMYS.f0k9#c@B%P:;,L#n]S=#(5H;#SFnS#,+.8#xji4#1YN1#M@EU-+g3n/pRE1#NwP3#`ZClL:B[AO0QVfq949i#am7VZ[MDS@v:j7eF?Tq2uQO]="
"$;TSSqYN'#UL75&]4pu,rD?X(vNk]YOYu*%)()F7=[v1^Ls8SR8KI58?JFS[[6N;$v+GVQ=S9At5>WlAvcgY?5+6L;onTfCKv)DNs^YxXtZBX(hr@&PYV9L##GA`W)fO&#HfJG2bRJ=#"
"S,/5#:;L/#;KV0$pd^o/s?N)#XlHK#i0>s/jwbQ#O#-;#)sql/CD]5#vlh7#tim.3UI(7#r3B2#q>MO%/t2<#VO+oLQWKP#ES$k$xsH0#66T;-:Cg;-nF=ONh>5&#Wh>%.@2%#MarJI]"
"HOufCJHW##t)5g1'hP5Abs>a3C^W)E9MHa<>@cs6mdjZ#e+@B=BTFTR#V<*3ExLBFxR:6]iO&[>>R7U.:0=tQZZNBtq)xHD;Gm*<Xvl0UTa5[lRV%%>'&?O^SVbC+AmwnAa(NOT8e%1h"
"uvj[PM8TCO`n/oSk'V+N5x7>$](]1UUfCP9k,YV.GHe]5%hE88('h%lTEJ88ww42U?&P&,0tbVRT.tJ23eq2(Q]l&,#-?^GHxLdNETnv?h6mpJ#'B',G<R^c=c2q8:_NQg<)a31$&13q"
"H(ZEXvts'#1`n?Q:/sK`GG1kq%sH'umcHkh$Q7x$;8$x6J2@FF[T@.3.4wqA?@twQDfXea:d&(cjKP_u/EF(5YfQRghcbLVZk6(cO/B:o1-(fs<Nh:]71T.j4,+#%WZ7G=gL)5L$]QfW"
"md&Adl-Bfslud.si;_G+^UjY7x^6GkDug;&@V$Z@E`D/NNSA5_86Z#.L^:mC=7^Mi@D4B$I&Gg3s<.ZIo+F/j@R<Z@U8JN;GrRgN=+>6:ZR%##`7`:m>lVxbRa&v#=9(,)]Ufr01O)##"
"G1+@'iK(;?[:SC#`JD,)ZUfC#ts;l,C5Im$jRjP$XREng$oG^Gj>hxbR<q-6t^?.6[t@;$bXD>#$x,Z$7SwA-1:#w.DwdKMs,p-$aIi?uTar2MNK*b.bQUV$8F*b.^3=&#+pEB-k1RA-"
"65T;-$5T;-]sEf-5#=kOo%FD*UedERqL##,$PZL-QXjfL>4pfL.L>gL.P#<-.M#<--(m<->xS,MN'2hL2'2hLUQrhL@3DhL.P#<-]Wx>-@M#<-6%Jb/(]5g)(Vg/)X5Y)4PgwiL*1F%$"
"23k-$;7Lk+Cr8auiXvl&4QGULW$`EMNIHY#C+ubr6%E_&,r9-MXe6YuxhKNA37X`W1?JhuSVC=M+&cL#vJ+AXXhv-$N]8cMp'2%$$b<lSU[Dm/'Q>G2cdA8%t]8j_gl`c)UNii_Xi+Z$"
"&]X&#i7iHm:%;$^tp1nW:n+naN.q<hMJ-F%=cs9)CA)d*?-ew'5*]0uPkYR*Q11F_&K=_/Q=+x'/+t/8r4+gLBLihLDVOjLTb6lLemsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M"
":rU%Ma*(q@?erIh_kHYmvU'^#Xis+jKQ+8o7U_$'jqno%<5D)+GbL50WGUA5h-_M:xigY?2OpfDB5#sIRq+)OcV45Ts<=AY-#FM_=_NYd,k[5'rVD*/6od._BwdTidR>L2p*'u$pn0N("
"YF(*47TID*Z#tj1o:gF4Z$nO(@,6g)f9M.M7Zsi,ix;O'dL0+*P[NT/x^E;$#0_lGw*Or'rnG2+G20ZL.*9D@%`tN90hbA5U;uiuS8Hd?[5agFqt^lJIq>[uw<+I#,vC56)+7S[lNs20"
"OW1]gD+`Zub2Fb+k[8[%5%Sj'%qldn0n-KC7uCduN/wM*HiojB(PUV$n`Y:m1402p=wfi'f*IP/<&buPKbIs-7>Bp7sJ@&,TQ<j1pl>F%(idl/eL8s7T-J>-/Lit-wJNjLCc6lLemsmL"
"uxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M:rU%M>Y(*%+;%Vm-4L^#Gnpl&xn23(:DEl_lr1(&7[R##M2QD-*c+p7FvY,*v@`$'Gvk-$WPl-$h+m-$x[m-$27n-$Bhn-$RBo-$cso-$"
"sMp-$-)q-$=Yq-$/da9VGJ)C&29U9VLs6A46X?M9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]c$J#:V29U9VS4(589mCp/.//tq(I6p/&UHp/4@x<(WPl-$h+m-$x[m-$27n-$"
"Bhn-$RBo-$cso-$sMp-$-)q-$=Yq-$(RV8_Q4D0_5HM:#eL6(#ef+s0<[u##EfYs-%5FGMfv]%#OS#<->X`=-,sGt$4nXc2foPL2p*'u$oNv)4HjE.3-+;8.GQBk0L77<.uiWI)*)TF4"
"D9<%$Zuh.[=$i.[p_g2uxA5m&`vP=(H:%k9)vEt8A,pB$%@^g1EZEwjD`Nwjib`[$DRV>Nl:Hf_9b)bSKarju'`,<-+_AG2jeJ&v`E4i$=sB'#L+^*#B6Y[$)USS%Q'#E+A7@F%9.MS."
"2$oP'W$6s.(KQs7r4+gL1NihLDVOjLTb6lLemsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M:rU%MT3F,%]$'Vm-4L^#LEMJ(F:)g2)$e2_tY?k_'Kv'Aw*VV-m_Js704a'M3LH)M"
"wU&0%Fix%+P'M50WGUA5h-_M:xigY?2OpfDB5#sIRq+)OcV45Ts<=AY-#FM_=_NYd=&L7''YHP83Kno.6B]f:f2l6#aUd)cbjJ%#2)oQ-*(m<-9&IU2Yc`B#';:8.(Q8f3%IuD#/jXo$"
"q]DD3?DXI)?@i?#kOsD#o1E_&V`G>#7<xP'n+</%l:Qn*2*CT%3(PA##o:>#b9AwNmawb2^To62TJgl1DJ`JXQc'Z-IFdlA,4Hx2M5e%4OHnc*8A'&Brb995Op[h(Jf'pCU;1.;7J`7r"
"Q:RO4^(io8Bq4wImJ6>[vBln2u7I]uj#((UBq3$FOt&F(f$gAGOSF9=4dNU7=P^R1mgQ)@uxu#-A4n0#e1LcD@#ki9@%vu#.]39^'9#,2#OGM9@p0B#I`7x0*d(Q/;LH`EO`DYc^1@g)"
"uF&'$Y9W$#8(/;%om$)*XR-5/T,YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]c)G7vnZ9W$#e*2t-ui]i9V2Z;%L[?s%npXp7*<ZJ(Zs.J3N=@8%T]WI)l&PA#>A0lm"
"f[6i[-whG72EU$7Fk:bu'9o4f?]``3)8BK&'hl^JKF-1ZX4w^J?I6kkogw^]'Xw_]P1NP&,<<Z$3eNb%_hr-$nqMs-uQ-iLPikjLWtQlLh)9nLx4voL2@]qLBKCsLRV*uLcbgvLsmMxL"
"-#5$M=.r%M,t1-M'0gfL%<#gL1<`-MhKk2(/@Y)4%[*G4Ke4`#KkRP/_:,i(@B#M[-t1k52<C_6uWXnL[QB;6ol5J1bSJ^d>t&v,Bv;5/(b2G9*2m+2wC)T.ecQ,$lpx/.@15&8_h9^#"
"_@u^]^']V$L^TP&QXjfL8)^fL3klgL92D-2ogtgL&<ZJ('P_._V/OJ-=_IU.(5>##9:2=-WfnFN2-7aN?\?RaN8QnaN9QR*N`%lO2,JQp.r,YX$ZM[]4b%E%$hLMv#cq-f_Wf^Yu/jCMB"
"YEGE#Ktoa%W)v`uU4fMF<V5N#SJk[f>ObU#SO=8jaCE>#Y[GP87cBJhn(+87<NI#$9NI#$/Na;%K$vQaKiZ&M+WY,MDQ/N-+]_10`QZ6#ATY6#?9]>-.V>W-`NrKP[*x9.VF3]-DNAX-"
"dTAS%&X1?#jCT%tB7Rb%8/T%tq7:kFwYFuP7gI>,W0A<-11VM3_8eh2dOg/)V43on/EG>,$@muPHp^5/^dm_##fqxNZ)>$MAwVf:?axQE/tjLu(x2`#d:w<1_2U(a'F:VZ:tfG*[xbw'"
"EeG&#(*`Z-6>eC#32(*MpL?>#s/0c`?3`d=sL7Y1R1bTuxAvl84('=YeC4_AQnJ]=jI%Q&#YVP&d?Kuc8B@2_1EkT`wcct7XF9:#J?G)MjYQ.%n2w%+P'M50WGUA5h-_M:xigY?2OpfD"
"B5#sIRq+)OcV45Ts<=AY-#FM_=_NYd0-+6'9P)##M22].G0xr$QXjfLH_'cj0Vrr$T;%)*DFP8/T,YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]cJk6W]g,p%#cnP<-"
"/a#pLJ9-djT*VV-laSs--Cvq7HxVm/%0Tv-XYIh(F09@#w<7f3Jc7C#vBo8%[YWI)p&PA#n$(A$XOKBsCbrS%3-A>,:0f],J4$ME4YF.)x`le>#ZRonpIk@O,uO`N.^KVHTWwh^/.>BT"
"BH?L<Kc-[n#4ts=iPJ+#]x(Yl4ax._alql&?Q[2VUw2$#NQw*.jTXgLg/gfLo*^fL'gnlf:Kto%``c'/71h0,+j9-v5be;-+HUV%LnM^2_/%&4VRG)4K50J3go#/_Z)UM'@h[gQ&FaS%"
".iqxN]QU[bS?;*F+`_;$?P55/,WAREjkRS%5qWs-C?G)Mvov.%gsv%+P'M50WGUA5h-_M:xigY?2OpfDB5#sIRq+)OcV45Ts<=AY-#FM_=_NYd,k[5'9NQX$KwL:#EIVmMYx]%#A82QM"
"x`tD#Ffl9'O@Y)4KreT%+>@8%I/gxtJnV5>>f'w-eeh+4;%C-<c4+,knF@K0eiVe5[;+b#2>vDZA:AlIR`q],,2SL(XoN6-QG#4?dFf5L:vrk7wX15/0p[)vvJG)Mndk)#'^Q5.LL@JN"
">-elLhB=)#Ba>#6@mP#6Aa.L,WPl-$h+m-$x[m-$27n-$Bhn-$RBo-$cso-$sMp-$-)q-$=Yq-$md7p_1V-F%(aRh(DN(,2qmj;-Hn,D-M'-D-Hn,D-J*d%.diPlL8*9nLx4voL2@]qL"
"BKCsLRV*uLcbgvLsmMxL-#5$M=.r%Mg<(q@)9&cjP$4gjCah05<D+hYadTP&NsB@?APsx4[4*;?[:SC#a35N'Lgq=%-'PA#Ynn8%3@l(NhZj?#a%NT/U&QA#ULNu789@m0?RA['2w0T%"
"+iG]4;ffn9m['dknbVg6hi@OGprWk1#hKv7IF0lj;H?5hp56?0$:GR0Yj<E>nE-A1xCGa$TPXI)Y=^N.6UqK2=[Ks_*hx/1cAA@Bch^s8DwAmgNE631])O.=4YQG*R-s/5fi0J)%/5##"
"OG:;$%v<9#:rC$#l`P9'mJxn1=eZM9x(ew'4+L:#(kc29KQV8&9-b<-Q,1w-mZbgLbT2K(Vu>$MD>6##-<(<-SO;8.4t@X-HImO(4L/@#pD.sa,EBMTrFf:Q.P5##S@&r/JlS=u9n/Eu"
"1]7>/KhQ,v),D*M`Ei'#k3*j$i-oHHdNE&ddpJ%#gQ2r7+-vG*v@`$'Gvk-$WPl-$h+m-$x[m-$27n-$Bhn-$RBo-$cso-$sMp-$-)q-$=Yq-$<%%<-Qb+g.xCa6#?<dD-&fG<-HfG<-"
"HfG<-I+D9.3ia6#d;t/2f2l6#=4B0%=r18.bnsj_iL9>,6Jb2_j'E0_T$Kn3Ow$:TPiB1&+jtP8_.hN(6otM(stC.35HUV%ZWsK9&evNHd4[U'UwD%6rMf`GoXom0BPqp;MV8%$]vtc."
"BlRrU]>fF#gv&e8a-1:/F^Z0)UtpK2f5fL<E0u#-ISPuuJZdG$h1c(5N5n0#S*,>>$0.^+;>po.K2V1pjXd;-&Z@Q-pT:]$X0Y4okRl,4so('#@lP<-G5T;-W5T;-h5T;-x5T;-26T;-"
"B6T;-R6T;-c6T;-s6T;--7T;-_b+p7fe6#6r3@p&r3@p&QbD,3v5,T.lR@%#rWOv-dV5lLimsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M:rU%MU.15%^''Vm-4L^#VV>>,46_A,"
"-tR'AU%E_&Glj>RhnSfjlOsx4jC*N0Oke%#b^m6#I,gKM?vRiL/OlM(%Weh2Enn8%jRsJ1`?7f3rT9Y.ae_F*Z]tT';>cD%[U]B-v(Si+DW,HX'tbI7lQ&f=tlSxX+@Gs%DHQl_Q_F<S"
"IoZw9m;#^2np0k30GOa#fpPn0m1@&S>c6Mb5^;t8sPri'<$B>5=(gB&lKZ'+*'[F4#F6/a[`<<1Tr7q/s)dlTi,0:/&.>>#1N&##h532gXQ;`6'9,,28tY<-)uKj%Z--u%jhYV-<TiG)"
"v_Fq#x61;$somvt^.H;[&>uu#L3&]kR_f/rNS8>,>c%&45ch;-HGQt$oPb*.c'At704a'MdJH)MSvhx$_Zv%+P'M50WGUA5h-_M:xigY?2OpfDB5#sIRq+)OcV45Ts<=AY-#FM_=_NYd"
"9Ie;-#pYD/QR@%#9KNjLTb6lLemsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M:rU%M?a73%C.&Vm-4L^#g0gi02=Xe*82e4_6A`3_lLg'`jg1_A#TP/::h<&5EV4p/hs=&5Amwi_"
"==3(&Ho4;6f2l6#]-4&#b^m6#Iom8BL,Jj%1'PA#7;3,)(xkj1QP(e;U-9v-fiu)4OGg+4gCaI+/=p0%t`[I)-A>-N07Ox9=V/n3siv1:fb`u9C_ZbJxu/ont[x>7%t8NFr>ZLe*c7rL"
"C(JT$RdVbId3x<0pG4R8'bM`@]#*11,9'=DGm2ju`,fA6kA^De>m<sSJDpbh*9Mr4Ix:8O6x4>#ieJ&vo?RW$kQMR8F'B,3S*+T.@Rv;#dq(M%;Gx%+P'M50WGUA5h-_M:xigY?2OpfD"
"B5#sIRq+)OcV45Ts<=AY-#FM_=_NYd,k[5'?g2q%LbE0_67[&M1)`3_70W&dWwv##BYu##D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-[b+p7JVMs%BTg$'%.P'A"
"^;d%7IN<R87)'Z-(he+%LhIP/bnsj_M5p>eZ9W$#b.m<-TM#<->_Gl$*0E0a6[Ph%jHuD#5I7lLa>E.3#;+i(<uTP8?t[?7=:K_4gf2T%#&*v#N-;41m+`)Rff^pAgSAm4(>u:$L]aV]"
"ws=i6kKa.=E=c)0sc''+kSB7lj+WM>?3sg<)t+xSl)XX.TO#?J[AVL1'dWkU2XL<8Fc,H=Iad`_eI_FHsG3EaN4dY#K<kX.PDw6#o2s<#aHDS$.6>##;99-mCKnZgY6<*eB$4<-'gTW$"
"l,UM*?9M.&'>0N(B-k-$oA?+q%+-$$(`.`t*G+59-^c8J,%dS.<6(a+JVH9.fAO,2*oi/)3Ki-Mwn'au(QW>MHA.%bl4a/1C5I8#w>(V$HIY##VGY@&`3V'83E&UDb5dJ(%Weh2*pvC#"
"FiC;$CrAQ$kdH4$b'S>#Q%n4]bQJrHZ^DM/T*qb$=Pxl>76>GeiF3]-4c1p.:k:7[ULv-$X(OHV)$04#QjR+i+,Hp%U+pA#m>r1.X@XGM(v;?#iKLA4.G#^t3I<)uqqv_#$o)P]<bL]O"
"Vhe+M@;Z.#giuf$/$x%#mDB]95vsY-:9iC-u+u4%'r8e?9>0F_h([w'?)`20Aa+/(0dfcj58IwKlA);?#ABA+jS,F%T#u8.t$1N(o[Vs-cBsJ:pp*H;ZDsI3.G5H4858X]QwdE4p_,;C"
"ZlRQ8[e1,pJ-oieA6<<T(n/q1].)x-Xp&uI?v8]8Z_]l0Q7N+h%5YY#qoju,>fNMp;1KV6X?4,%amK]=KQL&5;/JnN5qXc2&]X&#pf,F%/a_c2#*NZ$])]V$OGS_&7vC_&sD832cpS%#"
"PV5Q9q0;d*v@`$'Gvk-$WPl-$h+m-$x[m-$27n-$Bhn-$RBo-$cso-$sMp-$-)q-$'L'6hj8p%#G-4&#45T;-D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-_'(m8"
"+h3^,BTg$'`0V8_^2mQW,L&_S/Mdt7P*=&5UNt&5,;nx44Gl-$exl-$uRm-$/.n-$?_n-$O9o-$`jo-$pDp-$*vp-$:Pq-$S*=&5Xa9'5-Zr,#TAO&#FDYo.b:q'#AfYs-<;OGMZ3N$#"
"XV>W-xBv:p0jl,2r#:hL#&sK(_Zqq)?$FbjDsMG)u`uS/x,Tv-pJ=l(ae_F*%G+F3RLd4(eGG&#4]c8.-&Ui)Vg%%'i7?>#gLr<1H+4T%wYp];3+g/+)4NWTi0V?:D4C0ms,4l]nw(ci"
"o,grHW?cX]u-;5>E^Kc2^<9;?6#2GVd$%;AfgsUDLhAg#CbkXu+9MV8I*MW#FLOJ2VJaalt1pE#W]cbuPjOmuGQl[P/%JrmnKR]FA]#5/YO2s$`VL+]S/9)#r2[CsSv[fhV/>8%H5W]+"
"LrrD<V-=2_AGI?phEn##Vf+s0S>)B#(k^#$S@WP&&NR(s7XpKgMBDYN&EXom$)dF2$if1B/nj(adB%##@JSvLMW*uL]k8%#%WH(#%+De$U2/[B2osj_Hm$)*0A?W-+K;;9>Bm6#>*D=/"
"9r+@5CYx88iC5s.>/8s.iZ]s-9R-iLfikjLWtQlLh)9nLx4voL2@]qLBKCsLRV*uLcbgvLsmMxL-#5$M=.r%M]t'q@l2g;-Z7@B&SuD_&6c49#R_6R9oTMs%/dcNbwnuu,W/xDcA;:Z-"
"H&B.*=LFj0X7H>#(xkj1bDQJ([RR/:5;w['#S0&O)j5iag.MG#7Y=[::n-YB'2G1pl>ewH4Oc1ASos9/hlg32'WD%WX<#)5<%lrF)KVd46%eR/L`SmD%)AT.(cVB4[#ao&J8X[,><C(W"
"$5)*+xr$x**DT-#g_A&vjmti$CYH(#Nj1e$OE'U`##BiLpVG&#Y9F&#D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-^tbP8G$k>-BTg$'H%-N<T_Fq&@8);?`_k[$"
"3(+0;owO_-%.1f)r;u%='l1a41nQ].WQ4Y'A2/4R.nfVB,E@Prmnm05OcB4(Y-ND5bnCdY3&+n:S'sFA''^c4v)7e2jSAdFdW><Bj>`?`[[o9CnTVk'tE1$+%W)p/FJL$nx-o_;X%a?-"
"2ubno8.le);5YY#f@(##,,42gNS8>,7Y2KsVS559Y[Q^,H9=8%QXjfL<.Xbj)WGp./OEf_U;EQ1:E<'%%EX58/lP,*rcI^$/uET%5mF*Xk^XJ^voY]=v_wCQt8v.GEnO_SXD?V=2?r7n"
"Yg)h@r542gMV0dubSs=ri;QatPuFS9d.>>#_uo>$=@qS-n1X?%T>###`[<3_Jt<wTFYZ&Mi]7U.515##6_i=-pkTK-.%Vp.t/oO(m4hHQvbUUS@C6S.$v1Y>/@:2gA&?_[,o>#MqmDE-"
"8j`a-Wx?k=LB]HN<6Z6#-s%'.x/QfL78@k_0DJ'.Y:OGM%>PcM9.2N(X7V?u]ZpMu&;XPJOoIo@2C#lLt5bV0MOVS$ap2<#>ld+8qsJg21R35/<O0^=dW[A,=3^5/VR@%#>KNjLTb6lL"
"emsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M:rU%MxX&0%H=&Vm.:U^#<l%v#QXjfL@wRiL/TGgL/RGgLLVOjLZYddjq)ChL<3DhLUu'kLg1NejV<78.oJT>?^W8dk]Gm6#Nd8KM"
"B3Ds-x=U5%@&4p@QcWj1jYY&#dWY<%/=?P(V$vM(G$5N'w:%gU/N,?AQo*B@nKf<;EjTUJ$V/H<I`B-@8rfh9x'CQGVl/`uf+,8/DYH$Igj(tBgg^*^_R&XBqaik''*&q-rSvo/'G]Fr"
"6Yvk;P[o@#sT_7I87Y**rpK-%Fo92g(e3D<x-e;-Bs:P-u4T;-F(xQ-N)+K&.sZw'^apdmt?wKuxs+F%1%L:#)n`;pcSZ6#a]Z6#gim6#^.=GM#J_;.4*x9.DPLe$`/flfWl+cca>Vlc"
"$aOl/[hdI_v)^d+Tls,#]=$F8+ni8&<nY*7dP-xeSUZ##T7i;-Ar4G-H'rH-'M#<-D$$l$Zax9.gH`hLX`s+V^@t<a:ro<aO[Z'TDF_pLQG-##pmis#A]V&.FExo8dVX&#mgGF@Rhi;-"
"'SKw%B7'U%^RG)4TRR(qJkFR4f^$E7xSG`+N/eo#n)L0c(ORd`O=sT/eh,($V=BW8M-b^MiaZhM5Y1)N'6pfL7Zh5893F2_GPct7xGvV%v8Jw$ZdDM2*YH>#*9G3bQ*CD3Q(XpuZ.m7?"
"kSQMBXuuj#<b4rZK+Rx=8tqFDQKDB#f@(##]ji9DN?(kD]Gm6#Vun+M#8Z6#u0h%%K'E0M<8+F3OJf(s5ujo#9L%Wt<Ln]-$[c-6`rSiBrsj;-d_mM&n0&3M;:i`j=b($#;hFJ(@f7EP"
"8X[aj9BXS%T)tS&D>9w$eh`Y#+lQS%M4ci_whB(&<lA:)x[e[-&D:8.1[Qk%qq;?#NSjiclklful-ZM'V+#,D?@Mv#$bmcDHN;2g$XlV6IDxfL:k[`aom@s%j]#<-<YfF-OO@+%pET9V"
"*BnD_drZw'Y_O?.-hu,Ma;#gL7qU)NcS()NcMYGMG0J@;H#tY-jj7D#,I]`a(DkU_A8vu>6x/AtHV@stUpxvLY5C&vw$Ub$+[I%#;wOG%DU]0>^L9q^C3Z6#O)oP0Ysk;-.<dD-$@=g$"
".W%)*n<.5/T,YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]c+O.?Rpcl&#g<iT.;=]6#-V,52;OEf_S?oo%bnsj_VrSY,TdsJ2Gk<LW-hV>&e1X;?sU>^=UQD.3Co=Q>"
";)@A@4Q@vq5GN#AZ/hNS*LUY7Ss#YDUAM&6N=4gfQ;=QBr7[E^vliQDa9DtBf)MZ``_JtBU'<iB5SZk;W=_O;w@#`j$(b]4%[X20Ao42gRlou,])m,F@2oiLsl^s$CFX4o4'b9B^Ej$#"
"@lP<-G5T;-W5T;-h5T;-x5T;-26T;-B6T;-R6T;-c6T;-s6T;--7T;-gTZJ:wqW>-s@rp.lha6#QC<^ZR5_cj9BXS%ndTP&2wRc;$*?v$T#:9..uv9.wV>j9^#a#5Bak^64HX3Y;E^SM"
",Cm=A7`e12*6UUN8D>41lRY3BSj#1<#+:2gj>q=[3X]`.^02rZj3X-E)Laf1gU&auhA-c$<m9'#Li;<>fuL50#.e;-833U-=B3U-9<Nq-&C<L#YcLe$h+m-$x[m-$27n-$Bhn-$RBo-$"
"cso-$sMp-$-)q-$=Yq-$:<'<-vIA#%b`w2;n*XK;)6=2_koC_&l3*?@.+](#ocZ(#D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-]kF5818P)4BTg$'`0V8_d=Hq;"
"c^M:#WSk&#iU;W-`<^YADEs/:%vMw$V<SC#x,Tv-u@UKM5vX@&5;+i(QII8%qpXoL,.F%$'fJ7&U83F3.Iw`E_+VcX$l&b*dZFt>q$p,2dX.dlw^GG*L`ss/A/2lg'(oQ/(d$O'YvXtB"
")h(*3ows[kgapqu+gC]?=h))+iF-F8^'F>dE;5dX9u-3#-nV78BtT-4bj0h$tq:;-2)mT.D<O(jSG>G8,iwK>Ecg'`8+VV$uw[0s8%qnflakgL[j^cj`M-T.gim6#&`Jg%KE(;?&g6x-"
"09+P9=6T;.He*9.Ynn8%=='U%2iR[u-jH^lA^K<LttB@@C2Wq1A5Hi=uWNiNTY%_#'uKj$wrD.3d*`qjA]H+81S?G*=H:/2m,'##r3Rc;OGS58HeAD3T2.Q_m-S79k$#d3)GiG*1`iG*"
"PJBEuMZ3r7]H/s7<9i^-[nh3=a1f8^tuu&#_Wt&#D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-]kF58&SGs.BTg$'`0V8_PHT9V__1F_<jg;-'UP.9n&9v-0uA^="
"/3YD41tJs-QLTfLZG8f3'B`^=FJ3^.DS=e#DE2GCU42:H)C)vA)<fl;.@3I3B[7IKdw:k9].xhY2.hS^%&mx9'6up+B&jn@16Dc?2/HC+iDS/3<((;^h2N6p[_;qDtW7M(N,W-aACx16"
"VU@>##),##VY82gMu08.s_d;-6m;#21O)##fq%v#(E(;?eCbo@s0(<-bnuM(ZH7g)JHV+r63rrZT=(W-`Tmwk$),##9DDS$s&kB-^r6X$s_YY,MlX_$o+,*$4..ajJK[w':g,F%F=NO@"
"CiG,*<O7m'@8ERL&Y7C#<('l:f13v6KqD^4?@i?#c]Q0ci;#ack_v)OGb0&0jY6M7A2*4-*tQkie6Q3uZSt15nteA6N;^*$m+]sT.l)t8cnpxuM*LS-24H-.Eun%>c?=2_x9+_J3F/q`"
"j,Rh(+Z^c$CR<?#J93>##N`.h_.r.U$)dF2xipI1NXd'/hr###tP32giJ2>5qGAv$cICs%Zqr;-oT^C-ts>%.o#6aN8ZN^O_x;?#0'EtC9To=%TEtkkw<)ciLiiEe&*OM0r@-C#hLLD."
"$spI1nBMQ0nNHu,u[Fe6<0(kb4h0_JZ$%LaX`^._BUuG-js.<:s0?v$it#FI8c:J:D&'Z-UYfF-&77`%BvLJUEM#jue]Dci;#JV#u9e6iQ`$#>evTonoNI._9*n7.$,Guu#>8F-,=8F-"
"]$]F%6D#29s3Z;%TP]sp<n%>-)*o6%OJYY#SAe>%5o0N(N6EA$FZ4Zc_$hU#0(pe$^MLRuj=E-$;&Uf#u*MP-baDE-#KXZ/Q..1_N'X'#c,5<-tn__%#O>70&IZV-:eS@#;.0et2$f@u"
"X+F9uIcm4_J/qc]xM<_8jSHoIC`*H)t@Ke=EE8g:12bi_01l&#bnsj_Q<0La8Y2^#vb4u#(r7P#P7/E%wK*+M$,>>#ib)c`5WbN3i2k$3a8eh2&aCD3G'bOuE$.c`1taw0w0FoI9fQ.H"
"wfS*[4+AaE-G'#&dW%)EbfBkF:boEIU#6,%&B;hGnb<gMxP0d;s1Q<-e(0s&mPOk2pw:8.g:*73;[0ouVjCHcvG+h#AqbIu`n8KM-ndIqjLUAF(&O6;(?OAF$;[MKeHtEn^KZ)*Vm@#P"
"[C52_&F4s5X(viux>Z)*E2'##`.N%vk5CN$Rm:$#>SvB9cSCa4A8<A4&u9'#9CW]4Z1'&+;2D)+1RZ;%n']5/@R@%#(KNjLTb6lLemsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M"
":rU%MKEuw$2P%Vm-4L^#UM##,3-C&,Mn[D4Bm?#-;F5F%mt1F_8QM:#QmQdDGC*j10Et>?lv52':<89&ZdDM2A4DRU-trp$lqi?#c.Em/g<7f3@U9Y.XXZa$oR]T%j'PA#o;C^#mTxM7"
"P/bRDd#-K*$^#u@i4892S*;3Emb3SRQl$KDO*nM?/Y$fA]P7^6/a7,)vRAd)WQ8H>BP/7CH37C+&'`k1d0DPWR`*W%FE^1*@5vs.:r>W.'+*sDRuIfLSe6##<DDS$oX.%#L'+&#>gG-&"
"np,eOI1$##v:O#.T^pu?gaT/)&DXb$SMCs-r/LhLL%hnfKaR%#v'I6%9J<X(K[L:#wR?.2=b($#_EjYJK>on$=J[w'tJRh(2Z$x--hVr73DYd3?;Rv$5TjvS=fl7(9]d8/0P0N(Q+tD#"
"HBpw#7'$r;bv]HF^:GRb=QJM@KdGT[7F6uDE38)3;;=e47-&B,uQ+ip(Rx9/pIi?u61xbuK.>>#FWpRn0sG8IrW0rgce8`=@g@E+SP<A+$mk-$TGl-$exl-$uRm-$/.n-$?_n-$O9o-$"
"`jo-$pDp-$*vp-$:Pq-$,XNs-Pwo(M_=oo@P[>>,:T0<-L)5`%#b#^=v-3ufT^C]$NKLT%1qOlaI4?L<49*SG$waW&vQ9U/Xk&)]kNiJX4;f%>0b+.+,nQKjl+2b:h*Gx-K2&I*S,$4e"
"PU?C62(js%$>`Js6XX%vHp75%(R-iLO;4,8hVlS/1dup.iTY6#2oq-$G'D_&=2D_&#w+F%<m,F%0H,F%P`Xd=nVP&#2-wiLRQP&#XM#<-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-"
"*7T;-[b+p7v,0Z-BTg$'&5^$/buHdjI(b`*1FRS..W(;?s`IG)x.?F%,mZw';DSh(.&FD*@#hJ)Xv/K2#:Z)4M68q77D'N(F)q=o-ec'$1U2X7[QBG520Bc4,qX^6VKQuJ(DJb9`ugoW"
"d)g<STkNL)6mDR:i8$GR=j;e=UXj:mL+FGM]#Q0fS17*#a4W%vhU:;$ifh).L2a$8FDn21*iuu,K-R21sq-2&]ks'MAL.U.lCa6#Jq.>-v>@>%x9oj20qugLQu'kLRW%iLZlvdjDDMJ("
"p)bV$q.ho.(vV#-$n:?%mPG&#Qu8MB,i*B,pq0+*<[@p7]RK/)r``T%Buv%FF^E(j=Gj`D'2kY78+3lZ<%HI3W+`W-=a9]?QPw],Cw(4;d1P31rdJJ1KWPJ(9EkY7`lptC=Y*,*FVFqe"
"rcvj'G(9];dxO31iwjHEYiw>1A'+&#U_P3%Y_R%#$_&c$q<k<U`gpl&D#C<-5x(t-20QfLFA@k_<1;;$CATs%b;,F%^T95&u$n;-5R3N0fujh2wU7C#FF,gLQDfX-60x9.>@b,Y&EoI*"
"sm-'+.IrVC*m.lfL)KNbE;e<0MkFK(34m2U$to%b-oPcg7JYY#bLNr?6N(mJcj9D3e0=T.?^Z6##xH6%Dsc6)V*id:5.xK>lt[&M2N#<-t0G<%OkRc;HMP8/u*r8&lv.kt<m,F%*>h;-"
"_n7Y-Doe,t_$JB&ohBhj?wtD#q.i?#&0ZJ(9h><%^?2J3jFC8.sa)*4bwG?IfrFCK=D8Nuwqo32;%?%?N7aD+05`Oa+/1?7XpirKo4wrslnnx:fNHJCO[r8c/s9`uOK`o&B1x60a+[TB"
"m:(#,<wD:i.GG:0C.d7.R0ZCsnYr7#WWN1##/17%MR@%#>[rm$>P;jr&5qbE=4=2_Lu*wp9vN&`@t/2',6#t-]#fl8]HF&#bc^p&vV.,)>+r;-2M#<-&Kj6.r`_*<mBkM(V4AT%#U4x#"
"PwBe)p@sT9$e&[`scD/XT%gp;$dS60g+NCa_?C4fh%oKf)Laf1OwQ0#)T[`$W7>##h]bUA78])Z?X-m8^L$B7&:Vd'4>?I3;EsI3;f&[`7/4R$.>[@MFu0]X7wIf_6nSg)VZE]F:t]g)"
":&);?7uqp$@o.kr4o^nfuAhhLvGA1(DuUSCpU9@%O1Sa+f2l6#U*(g%rd;b7TV2>>QTY7C*@U@%k#jm1[mdh2B/d>>WZg;.N#a3X,+.-fkU@=/<)>f*SKW+*/Hr(NG,.R-c1Ys$Tddp."
"-HtO<1:hWMAB#l$wJvv$YwhH-Zhe/*R<b8.gGX,2P^ks-=c1?>^-'Z-@uT<&$dBtuFm&[`t('suiWk1:j1&H)l#&Gi'FKfLijEuLh#0nLR=8,;[0bi_+KJY8;4?t0a8eh2Vl*F3;f&[`"
"@.YwT7V8A$#:q'#^>?a$`M`3_<I[&M(iu8.S`w&$qRmm':-V8_`]$%'>lb$'#2D0_rR0F_.pt?0Npt?0Ec[w'D`[w'QJO&`]rW]+UYRh(;FRS.QN+KNS1CkLrr;ejN3TZ$^4Jn3#8J?p"
"$.88%`[<3_B6;b7pK6I6$DHP/82e4_3C=X(d[[&Md4?*&%Bic).fO`'<?H6:6uA,ME(+mJKsbh:jxlw9iD0[M=$nY6cK&[`egqrB2Xc1WnSZ;1`*v(NANdp/:(VQ(<PPr-6/^L;tgZO$"
"C6f-=QeGWBNMsqS8+*jL?%M*%tq60#%w5Y8DfDWJ<Vag8:kiNXo>ekiI15##BCDS$-2H-.EYR@>BBMd*mgpl&LS$T.pQv;#wACH%k)w%+P'M50WGUA5h-_M:xigY?2OpfDB5#sIRq+)O"
"cV45Ts<=AY-#FM_=_NYd3h;fO]4eF<rP0j(1k]5/KR@%#3KNjLTb6lLemsmLuxYoL/.AqL?9(sLODetL`OKvLpZ2xL*go#M:rU%Me(=$%=r%Vm?l^99EEkbjHiec)/j;N0af+s0@N6/("
"Z1;m9FOO&#rPd8&9k?v,56O1o,h9,)Nh</(UPOGR2GxfQG:q^u-Y<T#s:%@#rfd_uZBu%jx,5eg(T*##WQNhul&N@$Z/`$#1:E,8$Zw],?-iC-8Ine'4gN&`67RS%2at;-Z'f+%aHHX/"
"1jc,2A$M$#a^$q&@1r+;VCk05k3KM'lRiu7's19/:o0N(&i<I+bfNV&Pr)d28t?s=aW&[`;S)kam'g2OAnhm:[r98WZAH4o&Cop/M@u%t,3+cuIp*^au>^Cuki%ih%/5##pDb*$3t60#"
"V9F&#0@n;-;:m=&h[,(f'EC_&;BQq95?VhL*L>gL^k6o;fi^Q(d]m6#I_R%#b^m6#cE$,Zx'^fL*:YM()2n6#g9Ir7)9^;.eK,=(q52U%R$waAUN8x7mgFd>KmGT[-8D`7a'fQNmo'JC"
"wD$:9gM6/(&pBR:H:LK2ZtjlJXHtmdU=@*#JovP/`g1p@-hRc;6f-W-4sa]%oS&7.VUju>9;v`.X5Z6#E2PN&5pC_&6gN&`3l:;$i_u^]+Fl+MvM2]$3R*<-<<ri%%4_F*sTld)VIu<S"
"Gow?D?_BA,'/V,%QZ>)ui@mJX%4=v#_x(&0ORM8$aYh/#m[gK%gf`3_e,k-$FktQhcnS%>uD`D+<GS?>j5k3=G;c8pw1;'#bj9'#D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-"
"p6T;-*7T;-^tbP8boCp/BTg$'&HCpLs96L-xuuD%[6xjk2osj_ftho.Sp,'FF()T/fJS'HO,x&%HH-U%`g.6GIQca*>Ni$HdJn0+b-Jw@n/+4LB>r-AsX'0:cAf8LUnA0D:;SK5I]cm+"
"i/UB)M'4u->`&c3KXWK*xd<q..cc8axMYS5IffU9Ignd)^Gr%KM.q-3X),##%h`s#rBm3#ob3<8gxwP'C]-L'R(1wgPGCK*:dn*Nvad##G,oj%xBeK#b)eh2PNv)4kDsI3CnU`$GfET%"
"TfaO'EfBeT6HRT`$d)]XEv'I;Qb85)<r[4*S4FDEVTs`ulSYLpvhJ<.jNm>Y-aa9D$Wi.LLI=MpHt#&+bBO,;IRbA#JXn;-kDO>&@S0F_?ODQLJQ'cj=gpl&`ONd*w`IG))][d;sRp;."
"=-;B>BTHa3Y87Mp4><'BN(LL29%hE*o@s99:eX*K#abe3jhdf#*eSe?nlAq/E7D58$),##,>uu#<p-O>CFX%$tC`GYMj$T#4g;@$$meL$#a@1$pI,[.FPUV$k[bE>t@(2)lo2,)5^5rm"
"klVsuO*8ulc%(p%5q$h_=cX#$wVDeHJ.>2Bve1p.$,>>#t5(kb.V3_J&`YY%r;lPnR7N4o(?*huDZ@2'dekpuHa`kuV9-_#`dIH#x;/CM_YZYuY<`*M^]d##@nA9.e]Z6#k+i;-15-T%"
"V#dNbk`(v#4-%idw&0Ce1pGG21atR%xltP/$L5v5Ui,g25H$jCI#()$9nmfC6#9/:]O/g(/``Y>/I<oTEb^%MuUF`uq+N)%opvC>LNO&#;EIQ/Qf+s0Lg[%#%aCs-jRXgLk/eh2:]d8/"
"gc[:/BpguG.G1a4IjZw'm3YD#.;Rv$VYA>,mVp*%S2g%SkBxT%(V%],dK*v#8=oJeTQq;ISS==?T_8E+h'vl<IYQ>-YJLZ-j3UiDZ;u#-IK,)6[CbJcI%)*PWr44UG0#(P-W0RcV+]],"
"2ilS=:VxN:^5cL:CA-M=SGqe,Yu>>#%PDoIJ%='v:@;8.D+vCj?XHoII=OfLF:-##nqdT-mwv&N7Jm6#RpJKMJk?lLDu,s0JWGXhodF6s8T@l1RO_0Mx+vT%(ikwp[C4,nSw?/MMTc]>"
"SrGT'FP4`sV(ODOUq+K1U7b(NV?jG;Lj^c2So3-kXg?12Q_qq8-fA,3J*XW-Dex-OY+Wej+bs;-^Uju$A&-F%Ihh0,UYd=1R6.m0i.2W-N-Tn<`+<.b8hh`$p)g8.7btD#JYYbuY.[F<"
"1P_s-Xecb#l;<9J=2pA.fp<`+qtC]5(f)0*JXpxXFCsp/l6plLaG^%6R>5j)$bL,3M,4.).1MwC'V0[@f)#P9lbRH+vJ8f3AB?OD%DO_?aH*V7qh@1*u2<M2G.Ji*O7cX?nHF&#gk8T%"
"pMO1)8DeC#,c&K&v2#M$v1dF2_t$##B0AYG7QDPfa3vu,4/PoRm^/W-Nia=]hb1p$b@d;-ti/V-w:e.%-WC_&p'Fe+9hRU.(ia6#D-47/bjXd+4DWlAh(xr^EC4/2^>t8An8kM(J,l@&"
"g+]]4gsf2B%li?#xa`)+Hghc)'_SJ1Gc;jfrF)45Tl<c@Sb/W.f=hp7Ol3GHN[7&+&Lbgpf5p@#'0D0$lTWY366lb`vqke)Bg_o,B'tOS]/V>5*A]Z`E.>>#'0&##xjGMp/).Y8g)p%#"
"qu6<-cK-T-755S.q*Ef_Bn('#bnsj_?QaEc-qU)NKL>gL5HJ1C'9`D+$X^20fMi=.&PFb3&t3&/Auqk)60fX-Ynn8%e3]8%=HpC@_cic_S(H;&RV,WJ_WOeuDq4+$*i3,aQeSS7jbjM("
"9(&RThSXWO)7417.####gUs)#Frg>$wLb&#kCA;%`P`3_xfk-$%J>A%CNQ^$xPSS.>hr-$>QZ#8uFFgL<^.iLGikjLWtQlLh)9nLx4voL2@]qLBKCsLRV*uLcbgvLsmMxL-#5$M=.r%M"
"]t'q@$ib3=R41F_fG,F%sI6R(#>M'#dvK'#D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-dTZJ:1)%Q0BTg$'s:wK>3MHq;V<M:#r/LhL@l@iLuwSfL-pu,2#dIe*"
"9+.<-Ytll$I>eF6=XF<%[=8C#OM>c4,08u6J]B.*:>`:%lYWI)O1ag;,f?T.^[6b13&Ut@^Op9h7X2D+an]0/<sT=@^quqh8Odc*7e%l;%arS%oEN*EJs7],UqYUhTsQ31>wm?EL/ox,"
"Nk1RiU;sO1P24s<6]8oLSU$##]vlx4JxT8.`+3/(@2cGDIE42_2WcO-K+`a%l2mi'0oqr$WIQlLTtJfLYmlgL$@,gLs@cV$Jr`14D<8,;B5Fm'YmevnE#gX-8*x9.VF3]-@V=u%JQb]u"
"7(2cu8o%VZJv;PCtlBB#w][8u<9ehuZ^YTO_JNDu&)R0#uPGP&tCHP/2]M=-6o=c-e/kERWGK-QTR>L2Os2X%JABxLCV](sL4BICeq'tqT/5##r=W-$T,N<#HjIH#ddnU0fD-(#b^m6#"
"u?<jLkZU<9Xq%v#oH3B>82e4_1O(4`MF?##<'778[uT#$v@`$'Gvk-$WPl-$h+m-$x[m-$27n-$Bhn-$RBo-$cso-$sMp-$-)q-$=Yq-$JsZ#8DGY##,Oqv.STY6#Woq-$0jm6#Nd&V-"
"/:J+/B^Z6#v?.eZ1T<,<x,8)=<29)=+B<,<I<u6Bf2l6#JH`,#b^m6#7]*<8Q,<v6%&%*&'s;MKK.vG*w2RlDm,O88j*>)4x90Na^W<7836@sZx,Tv-R@=`#lm.A,jj6T0=.;M)Hk[X-"
"UJF#?G&%Cu05`[.R7p`G5^fQ0a6b`Hd+vQB4sfF#?5*DQ'1&sTW;[B08,Lp;AkD(@GR8a5/5DQ<PECVQD>Fi2db@x,f%Fi(9BjD+ZN:uA%)=M(cN=P&lS)k'wIvJ4hEk:.[-:o0`o_<A"
"D$a)3,a$feLU(`1:MVG5lQ^7/8*N9Np=(>%1J^p:QW1`5cfdI8PROAuQRpiB'Ym;-qFgk$x2LLNd3HjB`R)'Q3:.aj6q%v#.g_<-N])q/[ujh2ZDsI3'YlS.s%Uldw[lS.s=#rbp^Ok1"
"Y&8=%0*N<#EXI%#U7m68rUvv$bnsj_`/RV^K4$##FT.N%-Fp;8V,-@92p%)*9'V8/T,YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]cUr43Dx=`'#dwlW-41*YAfAm6#"
";Ww1%:O*20satYGJ+k>-g%7`&u8lZG1nfm_l=5o$FlFa*lM5iT%.HZ$2fbfCs:x`t3,pau@OYS.&+lQ#mjZY5s.QY8u]N&r)/FuS0b(qSbVC2PbX0E/Oi9PO*2_tuoxlN#l'cmu_mg%M"
"A81Q9Trrc,oW=5nvjj4^5;CLZ2l_$.B0AYGZK.)E;298.LwxCWa:d;-wHh9&/%_^H[5Z6#;Ehh$5m%)*uQ.5/T,YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]c#e,E5"
"w7V'#dwlW-,vEm;9#h8%m76R(^1NejbJcf1@Fn;-Vbp0%,mZw'*9E4`)6X3&=;gF4N@GPAF16g)F4eX$ee+T%258LXjhE501m3NXO>cBmTO]A,B6^kV54N<[HL8VX+)Z@6j?RP;0Rf=@"
"-]S-)w45UDe*G^,[.vX7+B1-2_?[<-6hHoI:pLk'fF+3;ZTcpBee#.#6s<=$JHY##uKje$KKOs-DV>$Ml[d##Kj0W-`A((&E]OD[r+M8Ir=Ha3Nhw@#%s6S#[blbu$s+uu:WqKatoIG#"
"rwD.qBuSU#O;Fc`lBBquT.>>#OV7>5P1u@tW2wh%4DU>?cx=G-4i4c-#W:i4K%.<.VF3]-Q?X4ub6?M5j9QwL%KH##9CaT..#3D#XGfC'k$c][pfu&#HSuP8?;#j_A$20a[Am6#S+29."
"D<O(jb>,8ob1s&d9;j(4-YD0_hNTg8>5=#-'Is;-A*(-%JA-F%7qRpKUXR?*qj'B#x&Y<-cvL@'F;P#6?>6rm]BND*d30A64/;p([EIu6Z+iQ2n4-G*G55#6[wRduf#0^'gt16;*rSAu"
"Lu/1(bBjA)8(F)#$uTB#@#vPJD/B>u@unu-T?CtL$V6##,Uo9$th)<#[nOv*s_;<-RVbK&k'YPAvNjQkvHEqjWOqvA:FUvAcvmRVt1EXV3>f'BDnQWB+b'##rI;4#R-a[$W)V$#B5w/."
"YCtX]#2a0(Y5Qlff<4gLD?Me&E)cBIX.<_=0c,[-rP0uG$o%2'EL6uM=OBwUk08O#m:7gUR5Ck=^ZxI_lNUMB5Q@>,5CQA%s>[`*,g7C-7uX_$B86<-vmbG%luoUJ?.-g)]r.T%:fTMB"
"SN5FPRQNa>le/A0Uptv:#wshEoi.D#7&7rS^a0kC#a=3;N[ap+_:<K>cXg0DM),##TP0#$,BrP-i.a:%4d%)*`[<3_S`<qMc9p$N<d7iLKEspf6'T02d>BK2f2l6#3_S6C]uYp8w)vG*"
"QW:+Z30Ed*2lms-Y-q)=X6Gb%pp4glpG.h%TwOA#-fbM2vYXI=*IYq.%G)JFBlk@K.EP(?5lGN(7%vtSxCeh<uo&p9P31?-0*lJ.8>]6:.=5=8^Wh>.G6;&PikCM+S:Y.#<SZ%bSq1,D"
"fIR<-pULs*M8i/1'uA.qTXA#P$NgrH#lhl/VQNhu=gU;$g]$tGcrcG*^%iJ)O3o;-J9n=&#OaQL8=7aj0K%k94+2d*vOg(%eIJn3;E?k_35#3)$.gfLs@8L()2n6#:b*30tf_;.5@j?#"
"bB>Pr_KBv$K@+F32q9/%:Fx]@?6&[`'+s%@U=)OH-$:$7h>&&tFLm[65tsf:^)2>5<5Xfh]f62'2;v;-Otb8&>pIk;'dB'FN'S5'1]Qs-1?V-M?J,d$W)TfhrC`Cfo,r+r`Y^4S$Xvh9"
"6=_`s+R<)MVap9$3Ds-%:-tK*HX.H3<8i,qbkfA,#lhl/E<O]u/KP##0)B;-F0Uh$TM8MBueB#$fqK&#rAO,M@hwajs5UhLCY..28R<X1RQ(;?c0xr$TPYQ'1-'u.bUPRANsZp./]NT/"
"+YhV$6uo2Cf;H+<Q45c#-iJ_4M28)'GoSE*xMTu6+2u^]([PA4,JKJh<V<PA8wnP'uPBLj;br?@bDZ6#f1_._0cWC7[Jsm$,jRlfQ=ZV-kax9.MG>c4x?.J3@T'j0CLJDh@PUx,;q=Ra"
"-d]g#:4K3'/,Z&uW%f>/DCtx#RLWK9lG+_#Jv-&4$RD.3.88('9ZMEopg%Mo95@/^[T@rL[Q%VLng;8veU.vld(,YuaJN7X)7l1Z%5YY#Ii`(W7kuWS7T,,2fAO,2;Ul##)LEcm03%*N"
"]mdh2:S,lLLe`V$9S4K1Wkdu#$ilL^_>@V6#GSV6%W1SnrA3P8flBB#P$Rl8>OwP`GKPJ(=].-#q1qKG4]J]$NiI>/FXI%#$rI<;M=%a+La,d*>VRa+BXD>#htbP87')d*DGm,*oSto%"
"_G--*:DEl_ePc'&$0c'&^l=,MJ'2hLZ'2hL#G:68S,=2_HhFk42E*(;]H;s%A?W8&-w3m'`W^H*3g(;?jp52'HOn;-A@Rm/rOs6#F`($#/mME-E+29.SO;8.DsY20*v98.fbGg)2>mL:"
"3`D^4ix9CgEY_&O$.cbr/3rZ#b>)jugwl.S;=eou['<`#pD1_>V``[$.%Haj,Up]+eY+;Q@>#v#cYXPTKq-Q0gxtKu89Y<UNJS>g-oLl/D<O(j#RX4oN'7w[6E`hLV`8%#T.v<-W5T;-"
"h5T;-x5T;-26T;-B6T;-R6T;-c6T;-s6T;--7T;-A[lS.F=]6#iwBS-&^*$%@t=W8KT'8/(Da6#aV>$MG8-##K%>2_OXNq2CXE^6GaTo/`f+s0S;F&#^41Q8IU`8&f8?m'a=ir7Ak^f`"
"I4&7/G_FiM1Rh284=ADt2'Nw%)$p7Vvk-w/'G:;$Zu'##7F02p.su(3KtGS7$E/Ec08HX$>u4;6mmS-+$am6#iR@%#gr+@5<:^kLLcbjL]5Z6#U&,o.6Da6#aM#<-YV>W-w*::)hpM:#"
"94TkLbvZ12gs<w74OO)#+sss9tsE-=&K;&.?W&KMGPqkLCi?lLI7wlLZVurf?.5d&T(Wj9'0<s%dl:X-Y0I9Kgo_B@q4MX-k9>ZRn*&;8<(L#$B$-a&pYvo9?11^#aU6^(T####`.N%v"
"l7Oa$M7>##2]Fn*+xIQ/q]Z6#wha6#e@v;-<,n.%<x4R*MU+Lsh,^%#Re[%#D5T;-T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-`6T;-p6T;-*7T;-i,8)<]<V)+BTg$'UZr,#O#x%#0ef[o"
")@$=&YP^:'phZx6o+tI3L*QL%wxE.3qe+`#qtnlm]Qm$fht5,4Jme&:F8R=7@S;9_cVGYuh5uc,'K=GA@YWVA]x4lfSDfL2#TLiFM5NBoRIQ:vpc+##]89?#*1MS.;t&:%umUiB^g$&+"
"q3b`<suWS78ptZ#fSc'&Dtji08]KfLcC@x-iB%+Mw?IU7`+wX.>Oi?#Zl'LMr1$##Wde;%(F?t0koKB#F*5N^?<P3'0E2`/Ko$`/SM%)31uac),4#j'pk7]#Hm(S&;$<.?7bO:22V`f1"
"P+^@-7U1v#`@5/(2G#i7&/&_4g?H*Nmm:l-_Bwmso_uQslv_7#JL7%#xsH38TA^v-wl*)*@x8v-igA<-`2fn$&?%)*f$.5/T,YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^"
":CR]coa.jih2#&#hE.q.;=]6#+oq-$S6+W-?hrR8KY7Qq9bnajt/LhL[E/L()2n6#A`($#<j,g)@S@&,l]uM94:+6'?5);?nHWe*w+DT.n*M8.4BZb#ocQR&hC7kr0$vb##)RIgMBDYN"
"kJdF:1'nWLw?IsmA*^I?e>6&@c@6a$vj9J#(@ta$e7(i>*xR]L;+3rQ$F1S7s+d9B$-m9B16YY#Ra*##am22g^PI0Y=U+?[X3N$#q6Q]%:v]gk-G'nf.M,F.lW(RW-JKC.fin`*cYhT."
"QTY6#`7sq/m]Z6#oha6#K&J3%e$?L2:?QJ($qBr7Hl&E#b3S8%Df'fu+Qip#W,OTuGg,NuU6.8uNGmt#:H:w*-$p5/NiVV?KbOfLPX-##manD$mo2<#+69r*-u`T.kqm(#;''c.0Vrr$"
"tHm2V>6Z6#o55)%l%&hcmwF59.)J2::MJ2:,BEK;f2l6#xoYlLtEm6#.X0mroj1B>VH6=164n<4+bkG;volS/F*+*2v&-v$_,chQ1a>T.:%lf(I[9*#+R4p%+#CC,/l#+,BA_n/?25##"
"#)>>#jH:;$#:lf(a&up89Pq'>[TCD*n'X`<uoNS7BXfi'577m8$)(T3@=]c2uF%Q/.sk?#AT:k';-B<QsGJ8#6N:@-Z>:@-hLUp$[<@dMv/:a<vd1gD8^p92h2t5(2MuR(hrNS7FkFJ("
"['4s@3v?]-u@7_SO<&##kLoEelLtEe;%trLe0b4^-mb?#?Nu2v:=.3'x>12'cXQp.U[$hMa;Q:vbRex-JEcHNZp)JNFM6L-(5'=9*P&:Dac-A=3hwP'k'Z:2Bqo92KX1_fd5.gD%'p92"
"$cmvMP/;hL^=>N-^u6F%eHIC+ss7U7.N0I$H>/_4VVgY(vd;I-TIY3b;`70uN5fNbO-*kLlvjV$jWZ`*)KuY>b=jf(.5aj;]AZ6#1^Z6#*wjr$FLX4o*F;J:D[^/)v@`$'Gvk-$WPl-$"
"h+m-$x[m-$27n-$Bhn-$RBo-$cso-$sMp-$-)q-$=Yq-$lQV8_-@s'Mp,aX%_^TP&*om;-QX`=-@fG<-B2v117OEf_Ierr$bnsj_q8B8%X7RS%hj2U2V:&a+rx8a3sujh2AUI8%w#9Z-"
"kDsI3H&B.*JQ`?%1h6g)ur-G?Z%wY>Z`k[o*g`4Y8KNi*Ou?9.$+.f_Hl@R&n$K_#M485AJu@7&&Qi'QK%P'5,l)WosV`da$/-7/Tb$##]eb7er[+2B?QlQ#E^Y7&Na9B#dMr%4f(058"
"r:K,D+B=_/w7Q_/>Sc1^=hk/1L[a`<A6:_#30Dn'mi2F&(PY>H]73R3Uj<T'O?2B#,lrB#W$H&#--1;DTI3MT.$#Q/M''C#(fa31>^ap9ex0LM0OTsMuE6N^1@9S0`FC.?$?qhNoZ5.#"
"Rnu@00ba`<XJuY#]$xt1%-qJ1$,>>#hRduG)l&&X9_Cu9@<Sc;n&h#$h9=8%*N?T.h8eh2Yx:?#66Z]4Eee@#Clkeu>m]fF$j>]=?2JP#9W<]=lEvW-wlJe-W5>##LKYf$iEh'#[?O&#"
"ReIk9i.I21:g;v-$J)T.0Rv;#$X0H%+mw%+P'M50WGUA5h-_M:xigY?2OpfDB5#sIRq+)OcV45Ts<=AY-#FM_=_NYd+URvRh[%R9UeDW%`DXS%3x:p&L.x],0I*W-Qm,f;GM=s%(Pcs%"
"w`IG)/Qwe+70U;I;:bi_ZFHp&4kI:.XYt&#<Vk2_AYC0_76ghA@8Z;%(?`Z-=9(,)XRm1MY6^g1@N6/(7Vd8/2cg/)XV,lL<MD63f,/J3S08@#B-Tv-a@0+*@c[:/k9OA#JTPX$w^>ju"
"sIew;T1s0@ITOI#n2[s<2#?J@:<rm3rixYs_?bM0iB[=P[fI>f[HdruVUhe54A4)*lkcGa.0XlS9[=otj%HV#d]i^uNQ+[N<2H`WvE%*u-#8C#Z####rt[^JVw2?#>;nx4KBr>5R?L/#"
"@gK@#@>H>#)('B-$J1G-fHw],.OB&v-aO-vNulTOFFD/#&-ex-siJIMI]5;-MDOJM/VQ##&qqL5mheYd,4G?%`=RA-;2QU8##5RE$?qhNRZ5.#Bx7C5C?tZ#CF*B#ocL&G$(F2_.G+<&"
"rI3D#w[FhYf79O;I'5[$pl^GM_DNMMLHD/#Vbh`./,,##25m9%SQ#:2R/5##jH:;$Av<9#fsE2CB+=2_;bD?@#+<d*CM_<-N'Gw$EZTp.1=e._&(N*I9?\?k_=BtpB&tWR%1EQJ(F$nO("
"CK8a*:O0U%&kXB689f2Y;O1vM07m=A9r<M2.KH7O9D,41j4BU@Dww6:c:92goIs31tMB<:>%3)+EvhN15EMfLPX-##Co.T$t?4&#'+Hb:oC5s.gtf5/44i$#U]Vg*-m.$.;4TkLpeva$"
"6`NBoiET_MvC6c*D&s>.>dTP&LQTs-FqZiLZFm6#ZPHG?1L(`$`0xr$`:SC#YO?,$[$^:/9/TF4@.I,Dah>g)7`wS)UH0n)YsHd)swI/1]P-=+UJ#$,:)x-FS.h7f=(kX)0Ls'6NRSW."
"5tf^6`1J,YRHFlWF*P@jdl#<$heM^4OoZ2=(uL0H-2O*,SPmo/'t-gtOqGlL3Wr`57=f@]N9,dj=H*w9KA=#PJR_l8:kat$gE7>#<K(n#Y[v++Stji0d&cG.tw8%N0AU`5vpXYd+Zx>-"
"/tx>-Cpxb-)<IF.NA2`PHcCDjb;ngDhE/2'5:5##+hFZ-04n0#/xZw^gmpl&e%m%=qGqv$ViUO)0=4)#;__=%w;Z4o>dK9ThmsmLU0L*#T.v<-W5T;-h5T;-x5T;-26T;-B6T;-R6T;-"
"c6T;-s6T;--7T;-?I5s-v6@$MQbL*#er+@5Po@32W)?lf0Vrr$/_][7cv52'8**.?mXn;-lr:T.Tus6#Ii)M-(5u(.?L#lL-XlGM&uQ12LQ7/NoAZ6#Zia6#8<C[-KQ-'J1+o(#jp)FM"
",j0KM[3E$#p_0HM3D_kL*ZlrfF-vlLWXOjL&*Vhj3Be9/]7q8.S*[fCdk1I3?DXI)OKOA#oAic)t1di=r%5Z)&clRKJlRf3%,o3:T[wBPS:2A]PsO,J&dC^/,co?;&QwMKS5[>667`%)"
"lV$r%heeDav0.T.HomdZOraWT[Uk70)_?GuWi5Y:$JGn:Mm7?.?805%$t60#*O>+#YwVG:sK2W@:N(,2Yvup.HUY6#Moq-$0h_O;C@a3=C@a3=RaQ'A2ni'`>;$,2:_Th(q1+8@?1w;@"
"a$Y6#eVri13jm6#h$),#b^m6#=o.nLZ5Z6#-j)6&CDD_&*K][<luVG<YgE/2*q6na$lubljaamLe<*mL#aW22_^gf1FCaQ&R0b]@uuBFE-_ub%gZ$n*m]DD30hFA#0`^F*iMc6<Cwv]%"
"FGcv#01HRA*op<Ao+)p_-0RvI74jK*/=$[/F2ljVlA$x>9hsOFMx>=B2#SOTVl_/DYGUR57oqM@Ylf:8'?wnm)9E,4h-,)31Z=#-9&44D]tG:&^3R1M<-nd)D(wXAR`+H,V(sd%:Nv6D"
"un.8/,=HZL%%KE+MeCa>Dcs'41p#R/Ax0lDW1ZZ=I_I.3l)K%6a((02a_j)F`,>>#bLNr?0sG8ITC%FYZAm6#X^r2%GE#,2Fa@jKJ14oAZv__&jw1F_ZN]&M>_90%eaM:#gXH(#hL_`."
"?n:$#2X7aEEJ6=1;W0W-:*w2j`;E.33$F:.]jV&HYF@l&Ykst-fpm`*[*XY-a_Ykis+J-)_/OD++Q20($$SJ&+VnF<UG<$6dJO##C9+7/*5[?.GNW390I*N1PCVN9U)GG.0Dv$#,H[s&"
"ob?X7,u[f1Te[K:s]b&#^6`uPGnrc'G%6j1P-$`-US:.HV(dN9-c/g2B#2N:_IF&#sIkA#`'hK-&F)e.neJ&vEv:I2G*F8%2G#i7';8_43uCZ#da=g$jcKsIDG+v,tCY>H7bs92`BcQ#"
"*Z=C/wSe&#s+-&M*Xos$^6>##S_a0XRF$##Rf+s0g6+gL`Bi(E8k`s%g$A;$hV=s%Woq0,u+FcM0qU)N9YaJ2TI1N(gU9T&8x<F3;f&[`=kpOOM[SS$=4F8%CvGPJKE%$$Sd*##FD#Oa"
"Vw2$#8'ni$:9_3_:.NY$^XD>#Hje##`*,##j6T;-$7T;-Z,Mx$g]C0_uQ-1,<@h0,Sk-S[V(f+MAfU`j,@j0,*6,F%&*,F%/%L:#mn.eZxER,MknKmAQR[kXa#F,Mi4;v`;1.t`SSR5`"
">[gIJ8-OB,g@pu,ii2,)352<-[v8c$U'Y4oV7x-Fp]c&#@lP<-G5T;-W5T;-h5T;-x5T;-26T;-B6T;-R6T;-c6T;-s6T;--7T;-k#sc;?jA,3cXia7ZAm6#?pb7%hMif(88=2_'5g$n"
"<j@iLa'd%PI5CW$Y0xr$i,J21#fH,DX`YCn@f@.2)UvHHNTv>Bv(D'#qIIs-e7l/CN;2=.Y97f3I]B.*;kRP/_'@lLSb3]-;Mx_4j`*i(F=[3WO8fn$]s`>,*f=)ixBtf(+k<m=l6Lbu"
"LpSZ=8Fpl/E>$3hYh`B]gRXh2bRpTTA`PYVbXsf#]=:nu%=>>uDp5&4>cOe9Ofw%9#%3%,bH`?BaZM(W&:7DY/6I`,oN@nX^<e'##)>>#%M5a3g?('#0iFJ(]9E&):)5N^`w1H*t,lg$"
"%frA5LDoP8].SD=/_pS8S?=g$7]Oi(d1LP%kiF>H^7N)+*q0_-g+'x9>^U-QUU3;e7g@s.d#>Z-:s&_]UZqfDtG*]-)5vY#vV$$$XdgF,.&5J,F/8>,`<Xs@d[L*4qUZ=0oc(JUoSH&4"
"fDc'&WFNP&FM?W-uSg.Xe,'>GkUbA#._Vs%1v<$Mnbm##=j]a*htNQ/6>eC#F>mA#=Qs%$cCiW#b_KF$*<UQpaMAX#`]fLVkx2,)A.:$$e&x@F$>>GiwY,W-Ha^LN.R'JA1ZmeO1*9c$"
"eD?b<Hg@p&UdOa<RHU;.a/Cp.BLd4(j$>.FjGPA#Phga#FMP%&vi)A$ksfslWjJVJ6Q9h<ePsC;xrE=KR-SbuAD9,5>oEA,7Z&&,DJRa=>iwk#64ZruWUaDbT*qHk%mn@#P(2^YniEtk"
"]fnahM)/D#wC+T3hIc`+N+P:v0G$YlR_C#P_]);?]m#N0?1NP&KrV]+P3a78O+GG.%(^JU8._M%/'-^,4R0_-MaaREmS>$#?W1n#uD&,N7Z?.%[fxIC5Y(FRc2#FRY/LxMMhkp%QgL]$"
"'It8.ME,/(oNT@'J.D?@hKmB82)0F_qWKE@1BZ6#lim6#%+[6%&m2MKn1c^AF_#OK7,x;%%@qdKiZ$$v(DYYu1'lq$7A('#qt)W-cmUpB`stT%rw)W-04n0#.oYxOm_(/:[EhKYKe8t-"
"CnFJCZL^Z-6>eC#hcU$MubZY#f?%##8K72g`h3Yf?QrhLAtL^>?)dG*?2Qg)t72/(4M`p.0Vrr$Q-Gk45..5/X8YD4ehbP9uMk]>/4tiC?p&vHOU/,N`;88Spw@DX*^IP^:CR]cB+:a6"
"kD>&#f3M9.=$Af_e/<'#bnsj_@*gi'8TBj(kV,F%W&c;-Y:S7M`x(hL]X%iLExR.2Yc`B#Emgk&krCW-l(v_Fj9b)<W,8@tv5hEQM(ve?>;Cp%GkScVVqPig?js`RS.3RDe&X:^%VAWA"
"bUaHgT1nD6x6@cDYaYn$Z0d*4Y_wLZ`,hU/&5>##E_@P%6_^sLETs$#'+`;=@we5''l=hYWp$Wg+xe(#DhC2_/fjNXZ7eL*-klgL.<E$#VrC$#T5T;-e5T;-u5T;-/6T;-?6T;-O6T;-"
"`6T;-p6T;-*7T;-[b+p7rxe5'BTg$'bB7p_'?Q'AO+Th(G^CG288=2_=Dj?Bd^M:#C0`$#tKe*.4`0HML4$##[`Ys-3pWd;=>O2(>&$0)kR.,)RxR,tZ5Z6#GfG<-j^pO03vjh2wE;8."
"c.9q@hd$x.I>Uv-@Na&d9iVq%dseD$`3odZlu(hQ.+C-RxJH8.Xoh]&)c/?R^uAdb48ZCsDH#DMJEJpgaWx:u>2juY<O-.R,(*<kxVhBi'3@tJx[DhD[?fI#s@.@#6['GF[<feu48P0b"
"I%###q6[c;OGS58e?&7/Yq$##[[Frd5YWr96OFD#A)9D5`A=ZSCLCP8x_PDEh.r92K6p92mXFrd.8Wb$..aG+x#lZ-iHMrTK/:a<dXErT6R=RMQ'oU7.XW`<5;P>#?[RW$-<Iw?*st?0"
"oN4OM58wlLPU[I-`a[I-]mWF.ECIs$7QOg27%lfCWr-s@F%MJV6[>%0Y,Guu-=/A$W*N<#3k^3_N'X'#8diR8vD5s.8ken(%Oh&;1WeA,NT9E#aKs?#(EL#$Ox<B'^eY)4IKe&8)blB$"
"D3b.D4Cb3*eNk]u'&FPg<G+33+@Z<0V].1:r]CM(m5I=R*+Ai3vw^^+3/=VO/8@MfJ2NH&S;qj#EsEfJ_.VO1c+JfLMO-##maew#;.t1#Fk[a*[lU<-F1Hr'laN?#:VE%$f7LB_7^qo$"
"5)>uu$b5TM5(='M?,G0P^vJr-l0'`=>`qB#*mUD3orIJLSwn(MwT<#P$mCPJmSa'#q+@cVfkR`aM'$&+:vhH%t^Z*70[Z0F&OHc*Z0f<-.L.U.)=e._*&jE-Yv@jL.'2hLw-gfLO'Obj"
"Xge,M48bmflakgL?=tQJ7[aJ2H+bJ%FF;8.vFq8.$S+T%]QCD3q-l%$0loQE[dQ310`]U/E*iAO<Jqr05L,/2c`8mAUKnp/ttlP;SB@9/lIT8:wfS=usg_MBQ=0iOluNgt*UOe#-JOfL"
",51[-8su#1=fwgs^+CLN88J4$`;@%%];@;9JY-C#&ddC#MK4xtVQ)s#M9m`-23Xw0Avcx0aP*wp`I>L2,,:8.'tVsuM[a-%5Y2BM))NU/XXJ1pnPe]+p-v:2#m$:2=9sFNv@QuuTL5^#"
"RiDE-wNIm%)Rh?BYYOvI%4pfLqT0K-iIME/+uChu*-1EM%%Z9N:M6##%YD8$@2FB-RVGg*LWx,MAF4jL)00/LB'KXUx&pcMuVP</sMStuO'LHM/Qs$TrKeaN;#`]M8G-##5oA.$%T[I-"
"JF+J%^GIpCA$n29^UbA#-[`8&'T^PWXn)$#E2c;-+l9c*)H#<-O#Aj%i:SC#<1h/)fQNv#,'q#$?+Vg$AuW4SnWj*%p%_F$Ra0oI5laJLb#0iut)f+Moq?TM-S&%#^acd$PJ:;$`[<3_"
"bhYQU)P#<-YiT(%UwU,MkZ0nfEMif()sp<1dmpl&:1]n3b@Jn3N@s'M5a?=.e]Z6#i[57Ct*q`$AILk+NhjpB+A'nf4/qM(;lDg)CFr_]DWVb%K>Cr$$M8GM%*0,WQRaJM'9k*%a2ba$"
"$,>>##t^]4PrJcrm-gRJ<nB#$DHt;-<.xE&RO7Uf9SbJfn#gp#qcNj0oY&v#*:)##7?4]k^nr;-qxKL(qX418PwQ0#1sdo@j#u1q55=2_j[oq)6i+r)n*HZAa;Z/(Aq6l)IWO`<f3Ed$"
".b*r$.NnD_&QX$0R0N^2XoNp)'C>,MJ86keFtr#NTgTCMgP]N/;84*v4bvs*m*#q.X2t+;vr/dblSY&#tF7e+F$EM9(>Ms-@?<d=GPF&#X#@;9KuSh(%*QM']bp'&sSE1LYB)iL0@B^5"
"HHUY5u]Ys-NR-iLPikjLWtQlLh)9nLx4voL2@]qLBKCsLRV*uLcbgvLsmMxL-#5$M=.r%Mc0(q@D<O(j+fp?9P.H@9Kfp?9YW+20i%VA5h-_M:xigY?2OpfDB5#sIRq+)OcV45Ts<=AY"
"-#FM_=_NYd4EO6'n&[`39^Xc;'#(-m26Z6#o[sY.9Da6#NF'0%MJ-F%n0^w'S]-F%wknv*R9m@-V#Y@&&0L9.Zuh.[Vj#50#D0`NuSZ&:)vEt8A,pB$huDU/]::%jDiN@ji/(?U9bIvU"
"n*,##MDxl#cgO.#vpl^A-B>,2S+EW-?$W#0Xg^P#;66P]&<dru86n0#3'0PSA1urRfd2B#f']V$`>=s%9I/W-#bc-6SilL^0>slA7qTS#tp-xc.;j-614d7vB+a[$/]$79%QO&#`xS$0"
"^f+s0Q/4&#I]>W-ccT=oe&g%#DCi9.Bg4/(Bjb;-&r:T.jCa6#<M#<-JM#<-TAWv5*vjh2cqP,*a^D.3Ss1u$_r;9/-Q/J3?D,c4oM(KCo9=]-O$F:.@Dn;%X>d;%tiWI)`LMp+H4;kC"
"6s`9O/q+lu3/.t2O4?'$H/M`=M-uP#?2&@(@u?0IxF'>c-xxa#I=$QOR1?`@YkL0QO+E60=m'M#XN4%:.Ev5EFv;A,$FI9ijF-DEJFpMB^oQM'9g,>>UvFs-v9$IMtgm##]X=R-t7W@>"
"uqVW%r>MJ(rWLk4#nx3M-XPgL-JYL:S)4m'5QK/)*Dx;.27gfL*<x9..j5A#Am?['7Nx9.7xpMB+1oS>vk%ZpGMZ:ulC]dm-VeJ(spu:6+M>rQhQY+MlR5JuFh=_HtW-'#[g3AF]>u'&"
"$=t(3q8l_&YkFJ(s?o_&Pi68%?Qp_&uRDM0Hhj_&^'cf(D]m_&W@NP&p3o_&_$GJ(Jpp_&vDQlJW>q_&dECG)T1n_&_hJM'2wo_&`cslA`=n_&oAsx+Sbj_&,=]f1$+o_&(em1gRMm_&"
")518.6+%v#1:<p%7BC2C9___&kYkr-Z?_R*WRJM'/wo_&I$gx=Gjp_&puKS.t>l_&cN$)*W@n_&pikr-&Xo_&GQ*`s]Pq_&xOdl/*^l_&m;sx+jqn_&n.n`aANp_&5?mx4o]h_&Z2@S@"
"qdk_&Qx3)WSdp_&Nr>M9?/s=YvGu(3,#5L#Z(e'&YmNcDNUDYG[-auG1@0jC->w9)hOe'&(rdxF.c@5B@$qfD?E`DFBKm3+HdJ`E<`W>-cA@gDH]4GDTG+#HO8x]GH7Be6@#Wq)E?g'&"
"S*x?0bmq=Y+9IL20a`kD1+S+H86XG-dnr.G05DEHCVIr1^DxkE1D+7D>ZOG-)HOVC+t`9CPqfx=Y>(O=xLh%F$d0#?DAPg2M;1A=*2N/D:YqHH0VoBAa6p:BusV=B_tUO1c:@bHdN4/#"
"t7mR*wI+L5d;SfCL1L>H=d/j1>1xlBLgD)F^/Y.H?7GcMhJ`]P(>4)#=5ix0wWt7IlJ>hFR.xr141=mBk)ZiM#4oiL(X0,N.[//32MVw0pPp,@C^Bf6F-GY>Grk]GeDY#ImPG&#Z&BkL"
"QVHY>->hoDRB*g2(HV#8BqW+R=FLwBQ]#RE%%&;H.5h-HrFt1BZ68W-+A6L,Q]jq)e2s.CQJX>H5=1L,;Q?X(:h/F%#D1DEHai`FrMJR3]-uiChqs?-Y.PG-X#&7MJRG&#jS3rL[<7I-"
"?MM=-j]mL-*6T;-8oSN-E7+;.b?$(#eWm`FHW(mBNAbYH-^6eQ29*eZ=u`f1'9/C%)go*H1=7FH.'FG-'rCVC11=GHpg`9C6J/>BgTV=B;kl`FX/4]'XcX,b:YqfD%e4[9+i0JFnx#F7"
"wG@>B;-OG-1v_62v=k`F>cnmB@7fFH-)M*H/V3s71*(mBlvdDFe9Ne$Z(.9BM7co7#?pKFAp:b8)qk&#'$v1Bkg#KWDQhSM8)B-#I5)=-3^c8.FoBEH[,k;IZ>bdGO3rs8rGG&#q2,T."
"Vgb.#e)R]-enlAQW]XjL2FOJMH^;E-RgxF-G;)=->>aM-k83Y.gSq/#E)m<-I(A>-1%'4M<X+rL>SxqL.(8qL#2CkLq*9nLSRG&##Z<rL3:CSMe@LSM-4U4N+@1sL)KG&#8xRu-#n-qL"
"JQhSM7mGoLY5urLr;`1N`NgS8p1k>-@^.L,Cgal8qvW>-=>Le6sO9MBt%ocE8;b50Mg,_JIVCs-=40sLX'crL:.]QM_fipLj]cL%]3s'/[sPq2LOC,M3CD/#LTf;-2aYs-tGwqL,(8qL"
">6JqL,iRfMLR.Q-W[MuM77S>-)cEB-@5)=-f?:@-@=/?MalFrLZxXrLGA]qL=HUSMOwXrLF^$TMX.lrLi?;/#$:T59TrN#H]Zt4JArsfD?bGR<aQm`FD^Lqib$7.-fI9)FEW`DFCnJjD"
"Z3E$#[Z=oL$`4rLYD0[82(6al?g1l-I3r^oP%co78:9q`LT@,M,t%qL/a4rL$mrpLU3oiL3+6]8(1=2CI^=<-jrO8M'/o;'6gi+Ma=B18Eu@5B$wr>HeLDJ1:%pcE.i@5B.b*.6^5DR3"
"qQGkF'HPZI<=AZ$q,JkOoc2R3gQV3O17U9r)oH]FpcG78>xhA5CjJ@^OqawPpa?+#OpdX.09q'#N5T;-J.;t-v/nQMiLG&#5Z5<-YX*+.:)9SM'WG&#IH4.#GgG<-f7i*%Nt[<-V(A>-"
"'XjU-j/[]-''>^?\?-lrLY)B-#`dq@-BxRu-&[hpL/t7n$H/w9)/5<e?uO9MB_;,R<NrOk+ds9X12qXoIe)J>HCF5)Fw^HTgD2oiLP3urL.;SqL<fipLsPG&#F[=oL:,7Y8aBo`=R$fs-"
"6`kgL0)%>8.SZJ;b1(?HPC-AFVbn92+qS-HT-x?0ob'90iZWmL-A2/#&HwqL,-#O-OA8F-PqX?-Q/cp7[Z*#HmRC;I9G[MC34=2C&6Xwpl),t-7Z<rLhRxqL/GOJMF`4rLL?1sL4FKp8"
"mqW>-J*YMCJJ3gD*lbk+4jc3=i_%88h^TJDW4TDF2;gM1e$D#H,6e9D:-v3Xr9Rw9A3f;-'96L-Z-f/M#7JqLEL4RMF30W$?h'NCb-exFaqU>H$W2E-mi)M-ZeF?-Aw'w-#g/4N43CkL"
"$lY9'm]2R3smj;-cS$39ScE>Hh_E5B_bA2CtFl8^@2oiLJPPA)9>,q9<et3Nf4fQMATxqLP/AqLwxXrLWEQA88<)'6x.2PMBn@u-^S3rL)+8qL96a+&QHViBt1ODFuMj]GT1p`F(=<<-"
"ZTqq9fh*#HYcX>HBKA_/dTS_/<mb;-t2RA-LMM=--o,D-7U2E-D'Y@&I2w9)Su@_J:c$:0'3QD-b9x/MTX/N-Ihp4M$u)j&..X9V<S[MCJAjaY1gsLF`OMu9;rk,2L#QDF>^.#H1HOG-"
"-g^oDB2w%Jl*m<Bs[wiCsc^kE>Vwr1w2WG--rI+H=L+,H2k.@H,H4#?$d,hF-98&>p4W->pWu>-?N7rLQ*Y^G40uM0xi[:C,f>lEbShKM%C^;I&l%mB&dCEH,-I>H#W89C(DM*H>CRSD"
"u1cQDn2fUC8hqQM*esLFoN]$0TdwF-*c%/GA]w;12rcdG//raH%vYlE==AEHQS.rL`_+#HfaQq2W-J_/.luhFDZfF-m@P[$:RlUC>H>$^`2m^=D+(hc-w[&>u+Z1FV=W5B;EY(]w`e9M"
"K'&mBvR`PB5wSiF4QQL-=_CL.sjjjE</]DFkS4oM<#IKF:4f6B#g(*HFefPB`E[./4o<GHeW0eQA_`PB8]RFHH;7rL[Oj+&f_Vs-5&5/MZ8oF3pd%d3%A.@-d%M*Hu(,UC6LofGT`[/D"
"B?rQM'GQa$8G+7Dv)FnD@up>H*%@'I]LN5B`#AN16G2eG.47FH-e3G-r$W3O*FtDP6iUcH<cqiL?t_e=gJJKFpa)SC>jUq1%B*XCsg':BF2i:1c;EVCcj>bR7%<gG$W'oDs1cQDgEvsB"
"8ln+H=I2eGsroTC;[M$pR08fG1qh]G3CkCIGG`t-`YBlNKXCG-Fb.q73KTJD>tMN18@=GH5')O=94`PB9McUClY>LF'YUnDxP/O43$(_I5u3GH3lBfFm)8lDq;xUC%h0s7s(t>-^gJA8"
"_gE>HvP$6B8*OG-lqTImE)^j1w2WG-N=4;.fpjjE:W<^G<xOVCL42B>9'FG-+k;'f&wusB`NHrBvlcdGI)FSMf3_`$Hh7FH-gpoDLjerL[[CcHule<U:2AUC6cFVC4]eFHZek>-u*g6<"
"9R7fGF8)X-W$k*.;@Ep.e%1GDqE7L#G-QY5+v4L#?,j+MuIr'#,ax_'u:H]F[qdxF*Og+M<X+rL6U2GDb3Mf5q_[DIu@vp:_RkmBNl[r1=Y[72%.jE#h=p8'r;4hPKx=hYu=L`Em+CEJ"
"OQ]/GtLE7%]'xr1EW.rLImGoL;B7^$kOv;-%JLl%)?m^oa1ucM;-kV$1ZYrHK^)dEvmNe$Ed9hGvv7;-8K*^GB#pN)fYk8;4Z?tfFDEJ%:2p?9v_Be6,EfO2?'crLFlFrLcxP.#G0f-#"
"$(H&'K+Ial2=^k$+CZ'8)Cm'8nKj;-Fc;>MBmn^$[4;G;1Wf'f2Mh;-eY[B&g=#F7Ff%mBR$QDFxtbKldAo+DUL]MCDH2)FQ>F>HR&^p7gto4=87LwB;HI-ZAI(DN[.1.8IMgN2eXwA-"
"Yb-r9PZ*#H87uDFFoK`EiDcs-()trLJB]qL'+8qLZgru9(Di`FN@X>-S2/&Gk*#jCWj0WQBZFo$f/q92jeDs-G'BkLk>7#8*3D)F(0ZEe7r`3=)7&;HP*I>Hq*<gMhY+rL'n]c$^HB5B"
"/3eiQX4TDFi0i'o>3x;-KSEw:8B%aFROW9Ve-TEnJp*58o,0ppjFp-#ok*J-rT$/8jW<p^/x.qL/KG&#>Bg;-s&LW.6Mh/##V^C-4k&V-L.;.MQkFrL:r%qL@/AqL,r%qL(leR;YLu>@"
"8X+rLO-lrL:CfC)kqV<-ffINNO4ht'UC'XLp+II+pUu.-tjJs-^loKO/2fr%GM:G;TT[AG*uqW_1/eUgk<;/#da2&G>^_MCTI1#HXek>-sdNe$SkJ3bx3x7ICu-?PH4oiL/m]o'+T9<-"
"$W3B-.H5s-0/'sL/x.qL-@1sL_FD/#V434Vs$11&s'Ws-#Z<rLQ/lrLIs%qLvIG&#/YA_=udo5'_@f+MQ_4rLh@Kk$Yn;J:*%<tSg$[lLpiE&.`sboLYnrpLlo++5&KWnD7.<hFDT.#H"
":IBDI'U%F-ckv@H2B*vHsDJ>BxB%F-;fV.G>*$&J9Q.F-Ipm'=1H)hDIQ.#H;AHlEV2lu9@8EX(ExOVC?G0H-Z,=8M&CwgF,BFVC6*TMF*'FG-PKn>H1u@+H32%6B'ditBRwTt13iCeG"
"l#oUCTe0@-[?-%0?,I>HG_poL'W--.Dpw7MKXCG-SU_#H$6kjESwNn/-GhoD3rw+H/;md.ATeYH*5B$.Am*sLxYfZM@'wgFN&xrL,:ur1-mT<_<Rw#J@'oFHn=G>H(_;hLha&iFLXVBf"
"5WwF-w=dWqRD$IH8dg6<;?lpL.M,YBXuuQNR?/)OmS8eGMdq8BrIRR65E.#H&6H59*QP-G74&*H7`4VCk)FnDlP#lEnAkN.87FGHbNap76/I210;a>-:?CmBT>FsLH,8,(:sQ3'*4o$0"
"'K;8BGQMAG;%=GH3rI+HLNElEn;OVC<_I'IAg3sL'2O$8:5q-6.^DtB@m*sLSh3pDMbjYCx*]d+#-wgFvKvsB-s-N)M:T5'o.5.-E,'V:ce$^5X7VB)g>MSCFOHb#`i3`-)x@VHF,w1B"
"/j>G2j&Yd(ZSQq.w)N=BYZIElM:$##T(+GMp]aIh2g^b%au4Dkpp3T@'25##*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#c9F&#gEX&#"
"kQk&#o^''#sj9'#wvK'#%-_'#)9q'#?H@-&<'v(35>V`39V7A4=onx4A1OY5EI0;6Ibgr6M$HS7Q<)58UT`l8Ym@M9^/x.:bGXf:f`9G;jxp(<n:Q`<rR2A=vkix=$.JY>(F+;?,_br?"
"0wBS@49$5A8QZlA(a$DW-fJcMxUq4S=Gj(N=eC]OkKu=YRH)AO,Wm+MfU-v#a*v=PCeG`NqHn@ksrCfUox3creaQ(st)7VZ`5Quc4R>MBmeu:QS(f7eQfi:di++PfiuIoe1#Don^w>ig"
"qOb1g:-r+DPn?VQ$(_.hbS8GDRI]1pm*m(EM(1DER=L`E2m(GVYq)>G['EYGbKAVHdW]rH0%]4ol2u4Jn>:PJsSUlJ(UFcVwl6MK%53JLTUxLpaCY+i0#Oo[CA-Vmq#Grm]f%AX3Nk=l"
"ZRl4]8Y,M^J9Txb8Sg1^ldPfLn2.DN@'5YcLwwCab'ZxOR/(WH[5KL2$SIF%?*+.NYt2.NNTCA$kwViF#-0%JaP>.3S####.Sl##5.`$#Qle+M'xSfL8ecgL4KihLNL6##&/5##SCg;-"
";CpgL@,IB4-`($#(9q'#QFbN;GIsaEhpf.#n14,M#V3B-@'`&OOV.(#=^:@-aj4?-@Q+?-_=RA-'r:T.h$2eGq9bJ-6%(%/(;G##_GL_&PD>>#,O+@'*w%##UW#3#o&/5#R_<T#*iAE#"
"s6MG#)#Al#9RR[#/rZg#Zws5$Nr9+$#1[4$`Te<$RRDS$iEY^$@vc3%$]'/%sck.%jmP3%uB?C%reJE%=5ixLrX.EFJ8<#YEho)3h1pAOn?aAkTH6YY/38##bjp5JLN<ZP9btYcNp8L#"
"4tM)sp>$6J=QOe$tG$XUeQuCW&uq-$loKS.Q[/,Np_nb4/nRc;ZBsr$+(0]Xcp6lfB4.F%)rm-$L4kx4,l2a4:V6X1G)-/14lHJ19UsE4I<Y]4#*6>5/UTY5b4*$6je4;63sMV6)U,p8"
">u6Z?W9gr?(2H;@=xdV@.P)s@OU/,Nx9ADX8$W8&#p'3UXVHlJ<S3PA1>:3UvvxIU(59YPweq-$@S+^#1p,powZK1p7GLm'osf'&[mA>#e_;5KdvS2(KnnD4(9DqMcsqx4&K[3Oqwl3O"
"<8Y1^neF+rg>v9)/K:rmD5.,s_3/F%mEt92Afq-$Biq-$O>9YP)@3F%JC3F%?KIwBF.4/(cEP'AZJ55/LwhP0P^A,3Rpxc3YMCa41Bel8k0C2:nWd/;&L7G;p^vf;ndVG<#BSD=xPOA>"
"w`K>?#s,v?%/dV@8dPVeE9greK2w.i]of>$&XT5gxAuRnp'05gOx@5gplxQETOh9DD2HJ1(b8a4)EE']p^qu5VXW>62FoY6K2w.iD)13(kRM:me,k-$aKc?K<Tk-$M&R]4?LoCWl&9Ac"
"CinA,JeE_&7+w?0qFm-$gpb1T[[aw'4oI_&6Dq-$*ZMk4NRJ'J$1e'/'gFM9Rcbi9$`F2:mB$j:s#sc<-[;)=q)SD=(Wo`=xPOA>uMk]>8]$Z?-_T3XObT3X.T^V$ZACPpZACPp2T-,s"
"MHIe-O7V2(_K.pS&8nQWdiO.h3^_:dPFm-$xthFiKF8qV+RF_&)4/F%(ni--ti1F%fL7c`[KXPpph2F%4M<qVAfq-$hk35&#Qs?90,[-?CZ45/o2jP0M<wM1[]6L>Z4E_&2u4PA^Rv?0"
"%4q^f?3EcDe^.pSg&_w']Z.XCm9i--;&#)bD0KVeYvgreC?GSfFZCPg&7vV%[Wnxld2b#mIr=%t,/oLg=QBt-Uk,q7nBpV.nYXN1pw4;6$l^l89PTp9aM)7#GTCq7CwB#$THH/il[ni'"
"$*0,WD%eYdf4]9Mcrl-$lh)`jPFm-$=%(cr?A.5g.vcYd6+/kbAfq-$)=8c`&uq-$0wQq2/-k-$gvD+rCZP4onp<;$F(m-$qFm-$JWtr$Do,#mu[g--8h%RES^0F%K.r-$%[kl&vaN9`"
".V3_J%iJJ(,K@>,.RUY,Sh+3)1>(2##D9:#LYlS.lv-8#ea:1.(6i*M[v,wLNCm6#I%,s//+4&#CB-(#iG5s-'aGlL$R%(#_o8Z.%.;,#%N#<-x5T;-#6T;-)ZlS.4_.-#^&wX./J<1#"
"Ko,D-XV^C-VKwA-e3RA-=hG<->t:T.b'1/#A7T;-KJsC8l+GPp%_AN-6OsM-KQsM-.j4g.QV0#vBtJ0%Aj`9M9w0_JrnFS@l)i(N7W%KaEb<4#wWdb89X:_A),qX:^`iA,jB'C-63'C-"
"LhRQ-o2'C-HO.Q-DKKC-DKKC-4KKC-F^,%.ii*$M_Y1%MgHER%2M'F7g<,/(V3v8/D4MM0P^A,3eb=p8r<v?94xu?90S^'8@._'8'-aQs*kSlSdhRW&jFJ8#=hG<-D85O-#4RA-A7T;-"
"&QtJ-&Sp%'X-m3+a>g;-TF)e.=)p&v^t[>/H;5'v&JxnLB],oLq`5oL$g>oLwlGoLxrPoLM>N(v+NC).B+uoLWDetL;*r]$TIR9`G`$'6*d41#npH0#E[lS.<a;<#BNYO-p.IY%i&.ip"
"S1<kbVbHqDSx.kbl[fr?CU/kbK`.kb#=0kbY9c&Q<sCl-[,CkbjPg;-<.PG-%oVu$<#C;$rss;-GF3#.%%E'MMYZ##84@m/PA24#eaf=#q6T;-L*m<-CI5s-3]>lLOkc&#)*2,#.lL5/"
">)]-#LU`mLrFfqLAs)7#IPF.#d.:/#lZ%-##.fu-'15)MKWZ)M-(4<#`0s<#mO/N-H79%'01S]=-m4&>s+ds-=&loLAG;pL^``pLV[2xLwmMxLZTT5#-i*$Mqb+5v0=?&MCRR&Mbfn&M"
"'_:6v]*,##'m:$#4xDPp7xDPp8MfM:oa)K;uH3D<vV0#?-FZ;%^*/,W>HTPpk3B-d,&q-$6Dq-$5uZs-Jt7nLvPU[-18v92T#(kk#ap-$Ro2E,SVj(vK81pLUG;pLO-F'MbgBH%DxNJ("
"SVfl8nd9'o.8ci9-&J2:l*;'oKDPqisrK(jX:I_&sMp-$,&q-$6Dq-$639T.f)+&#F7T;-6eEB-x0SF/T5V(vE,uoLf[rPM:<X'MgZ1:#2*ChL'Ls$#av<J-Eqkv-cpboLi:X'M3r)*M"
"^4X<#%%jE-c'@A-&.$L-HhG<-E[lS.h5=&#pfF?-Ycq@-g33Y.#:.wuceq@-8>C[-sDdXLTj]'/cjTP&dV)5g#GDPpwc9;n7p(P-ofh-/v)u2v(jRj2^Ve^$+Y@k=J_$4OHRh3O4sd?K"
"c^'2qKo22q9n4'oc^'2qM%w1qQI8Jr#0cwgAJxEIY;l&6DkR+8vR2QLQ+:kLfvd29M=t8p3@GGMM6uk1]H.%#tTG+#//5##TxTHMl>Rg/C.iQj&)(/:1SC#$5qAT.trB'#,JME/l7.wu"
"G:_hLQLihLbsfwu#N$iL[e7iLSj@iLFpIiLol1#v)X5lL8Io(#p&*)#g,3)#o%mlLr5KnL5<TnLnMpnLOPN(v72;,#S9dtL#UB5#x=S5#K2.#Mokx8#,Ud&MFen&Ml-F'M[MH##WAg;-"
")A$j9c@(a4_MbJ-cMx>-xu<;M%iHiOTxj>NB>6##oo:T.ktJ*#065O-p++$%#T<A+^D%x.v(D'#9e=d;iM>p8.c9_8+@Od;[[OA>/,Q@%+ES0#Ce53#P&rxL6W^5#?Tp0%;DFlf8B$Qg"
"$:Z+ir%9;-$YY8.i,bX$2d)wZfU(&#CFFrd2><m$k6goM(%###";

static const char* GetDefaultCompressedFontDataTTFBase85()
{
    return tahoma_compressed_data_base85;
}

#endif // #ifndef IMGUI_DISABLE
