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
        font_cfg.SizePixels = 13.0f * 1.0f;
    if (font_cfg.Name[0] == '\0')
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "ProggyClean.ttf, %dpx", (int)font_cfg.SizePixels);
    font_cfg.EllipsisChar = (ImWchar)0x0085;

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    const ImWchar* glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges : GetGlyphRangesDefault();
    ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, font_cfg.SizePixels, &font_cfg, glyph_ranges);
    //font->DisplayOffset.y = 1.0f;
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
static const char verdana_compressed_data_base85[56295 + 1] =
"7])#######7tEDq'/###I),##c'ChLYqH##7HJf=P*=[)h->>#id>11A^n42<xT+G>7YY#VqEn/b%lj;_==0KbAc97OQUV$6PV=BJsWTPhmQS%-?uu#<0XGH+vTR5q3NP&HE.C#AKk7D"
"wR[i).D/2'-/t71B`(*HUYI%#:LJM'^H7u/+-0%JG'+$,VQ4U%>k%##O_):C/x?N5Na*##8T$##M=BUC:@.4mKN*##F#t92f'TqLOklw5u.[w'#Tdw'I3n0F'Vkxrb6###H]CwK9B`UC"
"9HHPaoR4S2h++##LawhFTl0)*X(,##$I@>1QK[^IxrpU#Q_2]br=Ps/+>00F#@=*Rb(m<-1+^s47%S+H&lAc+m5f:dl7YY#UcFVC;&wi0#v:;eZ(m<-vc)f34,&#,62wN'GX<5&ANuN'"
"5`G>#:`j+M5>%R&*JW0(-5aO'j<TN':ZZ3'(dH*3xH$l1n^=A#:qv<(gJQ3'b>Tm]M%Y^(XmP3'%w>R&V^g6&D.i?#C#J4()MY?,a?Ph#ho^5'&.TV-mjw0#YhX0G=4FX:3HdE3ZW*+."
"%FL-M4_YgLkxV0(ngj?#LhuS/8C^$.%hHiL<UV=-2,vPM0o:p.#MN=.==r0#H&%B,BmXvMv7@m//<V0(L^53'MnGhLN6E;$q5B2#-PUV$KhH(#$dAgMTqAGVE+O]X8=aP&5oqV$1`L;$"
"I-D#$3u/i#LVo+#.Pj)#G]6s$D<V,#W3M?#;#8>-/rfcMG7-##>x'(#0D$o%+Vl##*#5>#dLOgL,k1HMUw2$#3x(t-iXbgL^?E$#.s+>#metgLY9W$#/Ag;-4YGs-sw9hLi^s$#2mx=#"
"w-LhL_W/%#5Ag;-9YGs-'@hhLn&K%#6go=#+L$iLdv]%#<SGs-0_?iL=&xIMmD#&#:af=#5kQiLi>5&#@SGs-:'niLBDOJMrcP&#>Z]=#?3*jLn]c&#ESGs-DEEjLpiu&#FAg;-PFwM0"
"Cg0'#CTS=#KWajLt+D'#KSGs-Pj&kLKu'kLJuB=#&0$$vH5h'#OSGs-W&BkL$P%(#Vx_5//NQ$v%M>LM'c@(#Zx(t-bDpkL)oR(#Zx_5/9m)%v)llLM,+o(#`x(t-lcGlL.7+)#b4@m/"
"C5W%vS4<)#_SGs-u+vlL`HWMM3UX)#mXwm/LM&&vWRj)#bAg;-fYGs-*JMmL>t0*#l4@m/VlS&v[qA*#iSGs-3i%nLj/^NM=<_*#wXwm/a4,'v`9p*#mSGs-=1SnLAT-+#nAg;-x:@m/"
"kRY'vdWG+#rSGs-GO+oLFsZ+#sAg;-'GwM0gpl+#gtV<#NbFoLJ5*,#xSGs-StboLPM<,#knM<#'ZGs-X*uoLUSW,#'TGs-^<:pLUlj,#nb;<#,ZGs-cHLpLZr/-#,TGs-gT_pLZ4B-#"
"r[2<#1ZGs-mg$qL`:^-#1TGs-qs6qLbXp-#vU)<#v/RqL5L4RM_X5.#<#)t-%<eqLgwG.##Pv;#*N*rL:kbRMdwc.#A#)t-/Z<rLl?v.#'Jm;#4mWrL?3:SMi?;/#F#)t-9#krLq^M/#"
"+Dd;#=/'sLmW`/#CBg;-GZGs-CABsL&'&0#/>Z;#GMTsLrv70#HBg;-LZGs-M`psL+ES0#38Q;#Ql,tLw>f0#OTGs-V(HtLPD*UM*d+1#72H;#[4ZtL&^=1#STGs-aFvtLUcWUM/,Y1#"
";,?;#fR2uL+&l1#XTGs-keMuL-2(2#YBg;-dGwM0@/92#@&6;#rwiuL1JL2#_TGs-w3/vL_=0vL4E%;#LO,/vESp2#cTGs-(FJvL7o-3#j#`5/UnY/vxkFWM:+I3#mgG<-kZGs-4k+wL"
"GIe3#Ljp:#nZGs-;-PwLtH2XMAU34#%Zwm/jT`0vPRD4#pBg;-tZGs-GQ1xL$niXMM6k4#S^^:#MdLxLw)/YMJ605#'hG<-+<@m/'6]1vX9J5#&UGs-Y2.#MOT^5#&Cg;-0<@m/1T42v"
"]Wx5#*UGs-dP[#MTs56#-UGs-h]n#M,go#M-q::#=#l2vb&Y6#1UGs-pu<$MZAm6#1Cg;-;HwM0e>(7#eE9:#w1X$M_Y;7#7UGs-&Dt$MerM7#i?0:#;[Gs-+P0%Mjxi7#<UGs-0cK%M"
";lL%M*_u9#[(I4vn+78#?UGs-7ug%MjFJ8#H6@m/eFw4vqC[8#BCg;-F[Gs-A=?&Muex8#K$`5/oeN5vNb;^Mrw=9#O$)t-K[m&Mt-P9#L_c8.#.'6v2x]e$ICZ+iSJw.iRMNJi4$K_&"
"UhV(j'XV(j1LRe$S*8`jRfScjW%,)k93K_&_Eo@k+-4]k6[Re$Xhee$`s0Yl_XL]ld*[#m.7uLgCA-Vm^nZYm<nRe$jYd7n^t);niW8Wn2=Y1gLuDoncE88oA'Se$d3fe$uCAlorAl5p"
"6C>lfV[xLphsklpF6Se$((uIqj/LMqH<Se$i<Se$f(T+r;L#PfcNqFrnSdfrLHSe$4qmCsnY2GsMplsek)3`s@_L%tOQSe$<K/]t$Y=atBUBoetcf=uvF=AuTaSe$wmfe$E)Guu)ob&#"
"kC#29Jb#K)^>l'&ene%#*JY##G[,F%J)###^$v=PiZmV#*YU;$=R<T%ME#n&^8`0(n+FI)(u,c*8hi%,HZO>-XM6W.i@sp/#4Y313'@L2Cp&f3Scc(5dUIA6tH0Z7.<ms8>/S6:Nx9O;"
"_kvh<o^]+>)QCD?9D*^@I7gvAJH%tBfae6DvSKOE0G2iF@:o+HP-UDIav;^JqixvK+]_9M;OERNKB,lO[5i.Ql(OGR&r5aS6er#UFWX<VVJ?UWg=&oXw0c1Z/f$/[=ZaG]MMGa^^@.$`"
"n3k<a('QUb8p7ocHct1eXUZJf@Rb1gu)Yah/s?$j?f&=k;AB;$M:^TreG;*#iAP##PqH0#VYQC#/$]H#1U5j#dO*p#Uv@=-FZF>-V5:?-#YEA-w)1Z-:u(s-'nIt-=ZXu-[lHw-fV+'."
"j',Dq#)Yuuk^,%.g+PwL3-*G#nuWB-/d1t/Op7=-BN4>--uWB-P=XB-*+XB-4Zqa5wxOuu]-?;#s[r0vV.*-v$J7%#1(`$#%pEwLaViS%<+a0lT&W0lp'OhkF%pPke8aOk5Rg@krEtLk"
"pbpEk-9UH(PY:7#Fvb?-s.GDNj=a*M*SP8.&>P>#+=H['TG6L,kw@,ruCwCW-fJcMwRq4S75j(N=eC]OkKu=Y-N1/(I?U.q:6s7RQ@AloH#5PSHBqIq@V`4omW:YYUR]1paYd(WC<l:d"
"BA8VZ>vViBn_t.CSm;JC`A2Vdiii7eiuIoenlWfCp@+PfMiSrZu@@on4W[.huhBigru(Vm?`f%OLoOcDQ@h%FSL-AF3HJ]FZndxF_6auGcH&;Hggx7I+hASIm/YoIrVq1KWgEi^D1SiK"
"x%n.L6uDrmqj*J_/64]k-hV(j3<o@k.'S%k:`,M^Eo0P]G0Txbh(V%bn@r@bBI:Z,3N)/q%k'mJfsj%kZ+5(#B,(YlSl&JCpjfiTJIQ:vo9b]Y25q]c#Q%DNX1vF`t38C&W/[8]Z>i7e"
"Je#+%/QnVR,a'kkf(=ipDLQpSnaUuY2:I`/Xxj+MThN4#r?Po.<2l6#J[+(.k'suLUnW4#ZRojLx(G1#NwP3#C'kT#V0xU.R1]G)h3uH6q*rZ#k<UJ#wle%#>i8*#H/:/#(M<1#J:[S#"
"%A;=-l@;=-/reU.dMh/#p`.u-/_%;O&PF_0b*2,#@N=.#q=@#Mth_[MO@M'#`tu+#LochLf1/n0est4JNrDcV1c_+`/`U8J2PbE[k0kS<E5gV-fJ#;H.KOq;U%`oRC8s:Z72@hMGe35&"
"ON22gB+$##c*We$M&>uuYuxXu(C8=#3]0H22'+G4KOC/)SknW$vSp?-A(u59f4_5B&LK59YWpF.9xYQ1EgkC?>jGY-L#n31_scL19_,Q#D^MZ[a*hk*A-5I-7)`k0@%u598qY,EU<K%B"
"NkG<.v#(Z-@%m>)LoF2_PB%POf^p5#F7kJDqKBhG1(fpAa$)N$4'BNM:j@iLjZ:tLQ,QND,K,W-82a<LSqV7WdsY?.4heOOHscP9OJ_@BjM$[BDwn9.KQx3;o&%tADaT`<Z+YfLWI3qO"
"m%3*PHRDnAIsYlL:jjmL<#YH=LZGY-H@HX00C^/;GiXD-/p%3N>D.F3dg'Z-TV$##.uQS%5e+/($,P:v)u0^#0Lit-_=F,Mind##L-AZ#XTs-$xUB;k#a8I$wX[[-6<Xq.vObA#?CTDt"
"Oh&]t#PF&#'lk&#Il98.$oT#$q?7X:uje##O5Hx#1uH8%?@Yj'5pZo@^Uxo@[ppY$bA]t@Hw*2A.ET^3HgRL2Mp^#$mDsI3Bvw[YZYqiekJvqd3W^F27@;E/N696$qW6IMlYLW7F.Rs$"
"=qoi'x,XZ#=@GS7(J:;$t_U6p&dX6pSWp>6G$kiM7E4jLLuV;$QP5'Y)Cmv$JQqn#pYjfLS#4]bG[XVd&v'KL^3=&#r(M`7nww;HsY6QJCuWRJ-'CV%rNIA,_-S@#a'ic)m2`0(jVRH)"
"nH7&,Shn@#hKe`*IGP>#cWTu$k_DOBUYWP&S2VK(PeS2'Zoa]+'lcC+U#Q/(b%l]#An8m&SYPj'^<tP&LpLG):^BE<C_aT%TL&B78>Y@5t-8.$Te:I3e*b30oFZDmCYXL2C$u0#hO(HG"
"9%vca]]*_OdFfo@jnlV-*+M8.6VP8.X$SX-T##H32<MhL#:#gL:9ZVd3x4H#iw<YDT>>+rXusjhx0lcu]@elDs;WM'Z#OR0E,%ipBYnw#,u[RN$jd%XX35HN%qeL#Q:L5Mh&OvMT%$v#"
"UJ>lS3LIm/-v:D3pwvF`w^ta%ABs1+Vbx:1=k-N'+t3b+DDd<.Y0Q<6$<e)4>/$h:B@7T8YRA^=$+2x?1),KC5wwmBMd'4C'[NV76A-D,hH]t-%G`m9`*UO1sfJf36Iws$5?^Y,5*Jk9"
"G3Gj'hR'a#Gpq`3oVB>#7Q*d*4XUv%1G*p%'ZFb+Ongx$(g:H25SC#GbNX,2d$Z^*qu^t@6KW6pvkf)MXus,#QQ&7pmnt9)Iw[7pusIO(Eqcn0?/V#AEnn8%A>Mh1_PsI31IqhL1[wm/"
"g<7f3*S+F3<12X-OEi0,w'@H)#JBN([YWI)?c[:/E+tD#h`C<c],rV>,L.q1Zd7u/bW^kPfLoVCDOH-;[:TP9uX.OucU(pEu9TSMEh_lJIR,[uw<+I#PnVc'HIRX-6EHg%_]x)XeL8a("
"&3;+r3DW%cK*xYu:rEX$u&/W#3xK'@2S4V?Fg_[uIBJ-#iS5/vq?@W$rh1$#Mqn%#(WH(#&=(&&Dnsp%kYRl':m3E*uwpF*?'x5/?2_7/.&C0:S.,`>U4hHD2Oh,-dHYv0Iu7p.K9PD<"
".V:v#IaXZ#Kt&U%.gF)+B^3&+'La4)tGaT7MEcN'<j%)*#T,o/n'Ft$=gs5(&#DP03),##*nU3($E&7p&CO;pPW.@5ac5<-KAEj$'cFQ77CPX(llSL2ALOG)?]d8/K)'J3<N:B#?cWx'"
"Tc[:/.m@d)^^D.3x#2*3a@wH][r,I]IbBP]i[[1$:Bv9MGWpk0BRJ8CK1uoC#&@8Rm+Me3DiJtk']gwa$542gxEj'&`jCX(_;LNZ:F^iZH4M`*)ZA#cD<]uGtHg,)$]VP&+1$29W_&KE"
"['jh)(R$P1^NcI+qj-u8,oe)EdpwVJxf-QJ@1'8Jh6C)G#P*T%;c3^4sqU@,Wb^_4WE6q&sQVa4@c=:&an$<&#0>N)DiEE,L$Z_-eR*B68vY;'h:ib$bI4u'Sk<;7-6=>-qV#/_`Wa&6"
"1%k%$vLG01=9a8.Z=H:7w>dV%H1T&$IAx$5V`,=7R29=-V':A$,PTY,PM559`E+A#*Mbe)iaW6(k$P122G*j0A=Y@5d`9D5^l^6p)rk&#$^EQ7hg45pjw9U)SpSg2ew:H#ru>]%M=tD#"
"F.i?#%Jko71x%i)Ei(?#'Lj?#mBd)4XC[x66cY8p6etD#x5/.Mf2(@-oskU%Y7w)4^1.G?rU84MW7cnumeN&-W$?]UOq<DEc.&5Jeap:qaKeLp[cqJD^C<048I.CB2Cp@,osDH#_Vi?#"
"'%^]Xp#Rd?UFY@.T*G>#aKXR0;9v#R8kbrZBtE?Fx%8rPU=Ji@^nMb=7Z#mFBr^L2m@E=1JTtw?Vlm&0%I#GOT),##H$XT#kY.%#&:ZU74P1v#9xl>#,d>,2X#.87Hi@Tg%.TfLG2Z69"
"QUMq2aw(JC>rMq2r59%#A-eu%MJa&+mWNm/M/cD<cD=/:3B)QAOX2^Fpo;jKm,PSImpPg(8Csl&U)[S%B(c'#3hh`N,uWKpuV)E6iJQ;7?&o,2vQl8/XYIh([faO'3o:Z#vF9F*]wEx#"
"9FL5c&*@>=(I@2;,fPSTJVNBe3fQJ1d9['lWg_KaK7&ciW5oUm,e;G9d7md.r<Fe#7Csd%.e=^6EQ_,)dV<.)t&$$$BUdg)Yof&$(s:k)/gU4)Or<6)kS6g#p'$8)*-xh(TDUWHvw#:7"
"0c_;$5TSrQ;s5X$=1s4(/@Y)4%[*G4Ke4`#KkRP/_:,i(2.a<UqtC062<C_6ua0oLGQB;6kmDV-[Aj&d<U*#,G/sl/'e`_:nQDV-ej0aXgHN>G1riO(RTdv%BRPJ(_;8_4OqN%bF1Jb%"
"b:Bd#8<KY&H;(f#XG2[&iFeg#xLf]&]JV?#METQ&O$FX$=_3t$v6iC$BHlj'lvLg%i,[-)Wi5##<^s5(Cn*9%3Q=?#Le.[#:[&Q&h7/t%Wfsl&H<lj0P0^p(HVt,#&h4N3ib'E<`j)22"
"?K*i22sli0_,C8%<1h/)(].C#@hTD3hJ(Y$-Jn`uFoAe$Q*>E#e1i'8lliJL,Dj_7wP>8ITY[1T'LuWF]acUh$ksLT4Gr8#mxOa3lJKY$5(#>#RiLY7LC]P#0fb;ZD.)?##<b?#XBw8'"
"00HG&Q6G>#Z$LJ%mYC;$&a/AmtI5s-c<.GM6T.AmQ:ZV-4t@X-if*F3k7mf_PMLOux%JDX/]A;6thmJV$`-xkGD4H2F:pi#+++&#DDlX77INM01;.&4NK729n_@>>8p./CM:RP&+EcY>"
"+G:;$C:mY#iMJ3/W#OHri/j?Bg&>?#EM=87Ju0,[[p&YG7lNfL6Dwj#cuUZ#rsJ%7CgK@#xfP>#39bD<,6mPA781W$T-4,288t,#vkf)MW%]'0iKrUQt<oc2c6RA-s=^;-PReu12?tZ#"
"-Wl##kF587TrRfL]l+87u#`Z-6>eC#s2(*MpOHuu.(96$rFUhLBl3Y7L2,##@_[@#,#w(#]rSE#8H=I##YWL#BOR[#[c[9M>`'CJ`H`22KxHa.(=)NNRk]_1,HMGu&__rdH2Gb`98vWU"
"Mp`%OCI-vP#YVP&2.co7vX+D#R@Cr&oAuf*g'_E#sK*t&9M[h*Q/xd)CvbV-3Flj'^+[*/FG-s.hrHv$<N1*sIkm-$1vlE[u(8/(iY(^#x?IX-?;or6_T^:/GF;8.v<;^mdaJY*RtC.3"
">n<lteZ$crvq)p%'jAtus^Z(+Cn5^ACObv-BjBZ-1AWvJ(xUxF),`lJ=q->PtfZ.LXDRLj0Ok@<][QL<2%#>U?2NI=s,>>#:f###?1hfhm]MY5'%nM9>C7w#(=#F#k$`w,=Y1Z#b3pi'"
"mh_<-,5c]FGM>v#;Z4N3VS@12@/-u@=GvC3^9W6pY``B#Vn@X-MG>c4x?.J3@iS;62RFA4hqaAQZht@$3SUqSUx<r)46uw77$E>#f*W-HGM-g$4<F&#rJ[m<ohw]#B?(K(cD[L(C=p]$"
"V_-S'2]<a,wsV_$wjjT'Rh#c,C/,*$<9Oa$D5VS(kEZg+ogDH,do:+$Y14U((gFf,q*hV(XoL0,;_u&#)EuK1Qb=J22vPt$A0fg:YM+v,@pbv,etP,*pq`-.@#1v-Gq@[#:E1*sKg?I$"
")a3@554s5pjHSm2e6ho@no'edro%&4nGUv-3@Y)4stC.3``n#%v2k?#6j]VQnDMtgh)%ATBtmaC''avF9q.j0)e.7;/FW*$%m*v]=Rt*LH2s;81h]+kbGeD87R>kDi$UKu&i%Yl$FVKu"
"BxR)#uBL]&'9=F+FF+70W$,701P+'4jZ6d3Ba4395o$T8XROV?7t=?>.AcgCra`YIHh'@H*BcG+f$fa5dBf`,etJJ1;NTJ18wg'-oJ$j'_6&[#CNFh:%?9Z-hZs-*>QK_,ON:m&k`Sp'"
"neBa#nPf5(_k*tm?Q35(I#%12)CM2M3VKm(_fl8.n&BD5:M3^,d/sM/a%NT/,a]iLWo=c43]R+%T5%&4VUj:8rTq,+w0_F*Fg6j^[RD;$?s6m:Ll:/JBR+r'`EVC79V:O11AY3LQ9wsS"
"[,,n0OWEphmN09p?a-c82XZJIJ:YoeI`SqCA=mW86v195B(G31C]'lDt>ECXOcY+=ZQjn8</Y#60CNeNx%It*moH(#s6Rc;09NP]YJ:5&CL1GDavU2'vCx5'aZE9'^1Km&/WTs$CUn8%"
".b,F#7.R8%q@t873abZ#knLw,GkSm&Zgtp%6H@%%5_oi'84c87*B-T%m0^p([Q&7pxTs52T/i>7/`1@6I--@5j7JC#hp:V$Cc7H#6o0N(XADD3DtuM(e6T8kq*W`#5,[quEUhR#$`@H#"
"N$C/:52;GV,H&##59:3vgAZtL+<bR0xdc/<Wv>V%FI(Z%h8P/*wp]Q(XT&;0neSa%.kDD+kHk#-[4s#$.?`w.Xg#j'%GF-Zm3+h:v59v-[uHA#v&P&#j[v##*=Z01$[+p%H7F&dmI:D5"
"XTet@IDF&d;f^t@_>[h2qLOG)?@i?#A&[U%t;Rv$r06g)?+tD#ae_F*wFkcMmP1'dRU&A<v_=@EP(Y41>aWe3gfEMW5x?KuaD/a-E>_h<jF>_D`6F3;-a,w3GO(o0MM'1EK*wc*6[$S["
"Ilc;$g2CxZ/i<T%iE22p'&R]4b:Rs8YDM4';-*i2Q&Ww.g=`d4<N>;'Z<)`-PZ-p2xKE20ZD+.)Ox)#$^RsO:wlh11SW9Z->:v##A/h?5W/6,2t#*35v?'3AY7^-M'R,@5Bkv5.0JQlL"
"IaV6p2(F'oXZ>*nR,.&4,9+G4l1F3.b:CkLFt78%_xb;%t]`U%AL+fhfv8E942ex@L5@L_7hU9&ovp1i7G%%>#qFaQBt9Ae8BZ'G@N1s;L`,E58X#^Q=2W7m6r`W8KfJ'b?5;=I+V93'"
"bDo[ufBH+4tqkl_a`EW1Tx[Q0vD`iUfAg60+.>>#iI(##>g72g#U3D<JO<?#Ta0B#eRYb#Oum,$.2,##HLmY#(5YY##<Ov#D30U%s3eg#+>YY#fw'hLaiD4#d8I12P[)22wUt,#[sHf("
"B6RL2T&@3pucko@QW'B#IYi>$A>CphKc+o[oj_fqTLCs-+7>##+2)S$dCs<#ej9'#'_@c7o03e3ma6d3K[/r85r?p8iPwe<]PEu?b;.&>)l#[?4c'(>B'OrASlfNBgF>oAD]jvH5M3RJ"
"R$*-*d9Ev#DmB[#XjP5&q/),)J$&%#=XJ30+UJ7/xxK*8)Fv@-;^?*3pjv25KoZ^3X]-9(6`9D5R$8I$m3H12m*387Ps'*)5]Jg%0Q8f3IT@l1]B'Y.AE0T.ZI)v#9E%#%0'PA#7;3,)"
"(xkj1w>.ekL:_F$x,?Qm25&SQ>NYJ#CG4V#^s`/PwZO@L</mI=$+]pBV@;%I%+]&@n3p+R5eN6b7DmwbUt;#%m9h.Y;xp[Cadg;6;%RLEu,p9:WoG[5Z4mLuE(c.5^t9g2owSb[g05##"
"'PVS$?+RN8+u@Z586E&+;+T;6:gN*4eB5K;nux?\?Pl/r@o^X%G)Sfm'x<XO:ib2O)R6F5&6Bi$%'&`q%q-oo%s(U*87>e217P(E8RG[B5D;EDu+&Lm(L>'KaW-J8%pg&EYA:K_4M<#N0"
"g<7f3I]B.*O'w68xs>g)Aw#F#Vsmm4KL%vP1Sccu]$vX@.Zp+ER`8c)8Hd^@a?h`?@;NwE(7H]@@7x12].ZlX3q9eEENG_9m=B58oWO7G)J'n&dS4A#)=CM2v*LM`U@eY,Q2oburbRW`"
"aHTp]wY3W1Tu7q/d[ZY#)3&##%b,p@-l68%<;t5(,akV$78-T%^i`w$&5G>#?B-Du7U<8p]uXL26l*F31G5s-6:x)M@[cS76(f_WC7ZY#B1LuP,da_8xed,4BOws$cZ^u$.mMw$Fixx$"
"fnU$%1*F&%YfZG$D@vp192b]%s*#B$'aL;$/;5hM2CuC3D3wGM<W`5Mu_A@#L`CkFwsK7nZPEhGLA3%;WP$9.u#i_$tN[8#c<b[7uPp>#.Er($-HG#>P_%/1u)#,27n+j'xZYX7e:]X%"
"rlL;$)&###ahlF#Nb,h/&x,@5mnu,#Hf)t@T*(1;gBQ=(XXiS%c3nk?R$=B#xK,r%Pk_jt%)70uu>`r;O$ng#McLrH^MM?pox#)Er#nrZ-l68%dg'B-.Nx>#9M7[#pQ_v%H-DT7;]+(."
"k=;mL^fxF-5`fF-B&5c-r=9$n/0AX-lP_PS$j-)W,FH$$7M$c`p->>#34hWh,u(Xhg@l22QN(v#0m0W$+-;G#U(1[$tJ?C#:@r?#-1?Xh$8xfLOh@C#@]/E#kHl3.:S,lLcQaV$//N-$"
"%HQG$IG#Fue5N]OcKtPuUf]E[tGq29_*lip`C5;-jeNp9@#1v->Q6&41>HD<jA8m8FOA#>h.ucD$QU=H-Bmo&P`>?#k6@],@SNb3grbv,j%)<-=s6v$[.9R*vv,r%2rJfL;x/o8qnF5p"
"&KU^3/:tJ1mLG12d`9D5g6RL2=Fo#eiuId)38A$R9mF/*ZDsI3>DcE5IuoX]gp]_58HFmEn.FN9-&D<t17nieA6<<T+*^62]I%u.L'vbFI)Tx8Y_]l0Q7N+hJEAsdNrP;%g*N<#`UG+#"
"8D6.%CwsP&JTcj'x(fl'sDl/*e@B#,,<fx,47$K)skW&-c-9/12pP,2XJLV.gn002pgUp9FX.b>B:]W$GFgS:clr<'0gXb-U;xB7UlF5*Q%E]/+M5,6ZguJ(,AbH=EJPj)8gG=$%IH]%"
"N0?*=Rg.j2Vh`[6ci-C#TL2R8llch:+-O?#YZlC-c-Y403nZo'nUNiK&,###i^#JEi+kKpJ0^p(6Zv2Ax&>E2dhC.3Dw-87qF9F=(IE/2^/qreXc&gLcGn>%6):W-';:8.*W8f3[]B.*"
"_;oO(vo%&4Qm:9/;)r>$>,m5/M'D.3)?&.Mcl;T/<E,6/g:+i(;YBkL6?83%88_)<]9+J:jSVI?8]m9ZiG^2Vs4X<@%iM=uaks_sd/kRDFZ?5&#r@6>Awno_v0r+Hi9GGH2fk5c-T.4E"
"beIG7*Y@*R2>s/NG,phu<DRG6+4j7mu>'Z-RdH?;YolO#W,.6D[@MduR*]kuAP[6DJ-mr)Bdo+H9cj__NqicH]4a.G'eUo7bq()+u.anXd5YY#=r###^k52g4=35&M@Qv@c7`f:x/FZ#"
"GnWT%YuK>#)Rl3+w]16&^1h>#EU.W$u#BvGdgp`3Rt$<&UYN5&?+Vv#Ofp>#KWGn&:[Wp%7%;$#4]&*#F<G##vOfgL/H^W$P0i_$W#%W$4r'tm:O3*s.$<F*Nw-875YrD5NKLh$?QQ=("
"U9dtax#8T%tvhR#T*Ia-ndK#$&DT/);AN5AY7V1KJJmY,Rw6gupi+Y#(F'b$mZQg#xV''#rrg%M`w'kLIG`hLknRJ3>k-K3U2)o&XZ`c#'DP3+#Nl01EsH;%*bjv#>3>T%7^u01ntQp."
"4Wu142%d,4.<*L)04_4(x=pkL:#'*),x,@5aMLp.^KLW5Bn_:^eA6N'$CuM(utf<]4v_`3Jc7C#<JSq)N?Po9HFSb..)#ZcDxWr90r<NusGaWWW;Co:0?Y,[x0]p/)fnvoE7#KF(p]mK"
"ZHk*OO?nS#2v[74[?Qrdbrw:8JNuv*Z&ECAX>FX%jdS;$+-+`+?iq4)hVjGuRX/%/v,8+iCY(3CB6]uGTrtM9?iLM0ehJm:?q8q%TB]@#quje)SgJi)3#:^%A)mV'h8Kv/Q+bQ)q$Cv'"
"&E'+*DN<l(JLW9/(9***%`On'6eX01uQ(41**p:1jo)o&ic1*s1V(9(@k,87L'i0MYsQL2`DIQ'QwG&m+1_F*HKcD<x4U)N.UnM99a1_6UI>kuJcajEN=MH#=Y*vp3q<sqr@#':NlJY8"
"s/f*Q[?waE:.P.)Z0^ZApPDC_bRW<BN/dWo*]Ck;3C1ZU4=Sq9?_]Y#A**##]i82gNS8>,p`;W/u`Pn(Yi7='$=,Y%#7g=%M;B>#g&#;HJ`</2WYtoArojI)8k$/MF[jfL*K*7%Fcto7"
"hu^>$ACFT%OEQJ(KG>c4p9<5]v'xgt$dsL#uqN>#:B5cf6OTcc^rO3Q1h:iF-KdouWYW)5q^@d4sR9%i;`E03IWK[07sf;#IB>qVi%5wLT6]T&m>$?.hNsr$Ax-s$e#H&4hGv&40SG##"
"4Vl##kqc3'Gv?H)4]B68GrZ>6$1bV$uZ)229vXL2*c?X-7p0.$@v,3u$p*9uU*C-M$p32gA&?_[AQWRE[&'RE/kT`3fQi?Bh](?#k7R8%(UND<6V:v#Lqxj'6`hV$`t`@&+9$Kr78t,#"
"hfe)M:*sE->$oj3'Bh>$]8@[#>P0gcY?.W--SMPu6K]]%D7]uGYV;)W92TS.lw/Q::=ns$Z2$v$t=xP'[e,F#'a,V'TP;[&m3lY':]It-hl)B##F8w#9`&c3-Y]c3ZZJ&&#tRt-u@vcE"
"#Gom']1a^#oE.HDsqw+%(#xTMGdT5p^xO128lP<-'7I/6Z1s;$_o;T/;Od4(%.X@$K+Fj1%c7C#$$HJ(eXBa<nZAX-w_4)WY+u[NlP>(DH-McVgqqK#Lwmw,sJ3?[k9^@k[E&s$5hdau"
"XWsIFpqovAm3r`b(<5JUXdd#+)KD50%#*ipl/,`a+_n%FawN$%].dlLM+JT&l;$?.daSS%E:eS%AX<9%47*T%BL.%#pqc3'#N^:'EW_G)%i]g%$NH(#ptnA6Dn/Q/Dt@X-Fl*F3pk[J3"
"(rjRuw6lo#X3Mciur32g$?8/:XR^%OPa.Jh8bFW-_xRr%cNWv#dcZC#Z6j@&Eig>&A]1Z#O+Jx&Q:*C3bKs-$nj287-5Ph#-L570H;oO(]H7g)7xg_#pDMpbN/<eb%ejcVZ6/lAd(u21"
"Q=s4o/wYm/S2PX7'w^lJlc2F3I@w8%o#5p%p(=I)lG;l'Q1lJC8_42AKoZ^39gR^$CuXL2F+'u$kl.U%^RG)4wu2,qIe=R4eWqD7xSG`+N/eo#n)L0c$+;K_K1sT/eh,($$`X'Jk9*##"
"KlFwgTQ3e7_ZDZ>75uv#c+/t%cNew%('V(&qGPj'U1PZ,jKWM0<40?5F:o&4mDFJ:.wPT@]HSZG`Q#0(+DY>#7h1$#?3cm#EXN5&?`0WJmpjZ#IZlj'+6>G2h^<;$u),##ACw8'Lw&%'"
"62q23,a9D5Eh4a<%IVdauf_%+q27DNpt@T%ovhR#8G;>0=?1K(F$nO(^EaV$@nxXuN9?oc=:CWunNMvUUg22g$u;p%kYa,bB9(sL&DHZ$?J9Q(b#?&4,;###A.)?#S6R`Et*kKpI4H12"
":]EQ7Y``B#BD/[-B_*]=/&Ap$hZtL89Vm+sJ]Pp@>@>a73nF?#ESS]$*GLf#PMHc$eR3h#qX/e$S0;?><<hs?CMX*$o@A&&7+`Z#3c:;$p7j@$oN7D$F#r;$Bw&q%6C<T%DI.w#>L.%#"
"r')O'YY#V'E99:%%P099sh0F*L<j/2-17.Mq2<dGjlac-&D:8.(n^T6poXV-:Z6ouamAfCB0i?#35wK#;,^xtx'sKtfMs6.YdRJN]pHlL:(Xx%sTc/<]2Q;%UnU?%9mTB%o9CsH:4r?#"
"KNXA##PJx$rae%%KK0'%pVm(%?rC;$1QrG#-TlY#97es$31wS%3rH8%eZ=^&3*@W&>k<=$sTe0Y?(Zq7k5F^GOou,#udm6WjGQJ(4-,irpF*R#A%*X_$0dne6t:o7?$aY##v#YlNO0/q"
"^O18.QSq;8X(ofL<YhV&d8@D*hC9'+:Cou>7N)QAABL<?ONP_=umJ&&$ej]+:DcV--Taa*)1)&/hvpi'wD'<7)Jpf)ZZ4Q9kb26h43Sm2^a7H#QQSO9FMo#I,^B8%ih`$#>>.T%.AA(#"
"XfZ;B8;pftbTL:vD7#wA3bbV$P8r.::tM7m)rdS%>`G8%]@^dusL`mu.G@qCSXrH#`PiQ#wV[QDBteH#+/<`uqZgqut@nS#i's&d?S:5S`+3/(G*I9ith$a521&/;E3Yc<OckKC1P8c3"
"A-t9%<_Wp%'n&w#ek-X-w,ZJ(1?9W$sD>;$nl@m'Qf2^#aW#r%%/p/j@nN;p:Qk29+%%l`xCrB#I8UEY)EbV$`PMT%:)$B9xQJ^VFhCSRW`mE>huCc4GgT,c_m&<2796j'gQ7j;ISGn$"
"6(>C5k?XbG*kS@u[u?L2+keh%']+SImT1/q$NYc2KRi8BW,rg(a4^>,t+Fi(.LV@,F)Z;'R.o5)SX9/+,YUG-cqQMCIM.>HI^cs.29lP&OV.v$%ZUpIQen[#DQ16&Z+Z##Zxgg)M:ns$"
"n%5_+b;oE30G?%-w0G%$1keHDPSjj$Yg7F*7&=3p-`pc$Ma^p(nJEQ7MR/Q/;/i?#a%NT/7F]q.qi@1(Wjo7L*+F%$mV?30apuc6r]_a*'smPhgMPA@%YGO$QSJBG/Qr286,1^u=a<Ea"
"t9'p7H(u6Bt5KF_F+5]O#WJ3+g7_*8IV-Z$DT$6%O.Ka7HHA>,7iWY5kC-59,V-1:GCSY>e?S_FE&nlAr`DNKaaYK(/_+6#Jns9%_TLa#-chV$$[av#@?Yj'p5tV7nPNe)Mq82'ajF&#"
"@*Xx#Shv`E':4C35x^t@x4_t@X#.87Iq'*)8Q0j(N1W20u%pbaV<bB#=#SW&xkXI)F_a.+,82U%WL%mS/O]i')EQS#I16B:VcVkPgk:o0GP]Sdm4YO1IR5Fu#_JN.ADNHI,`:D#7pG'."
"$R_Of_-9*F^We2K?f.p$mm2<#$wec7VArv#+@@a+28D50:s'V.$Z4C5TwT$5lgQD4:No29*8(49MUnp8Whtr?Fs$A>0QuHElB7iC)UniC^3RnK15D=HUCO?H0*HJ(1f^>,;G?s.+7F5&"
"vhJx$66wg)d*8%#hRv##a/S*3&o+p%2vmCdlq;E2A1es-[(s_=m)pFcCx;T/PgY=$r>GW-W6+J4$.1f)F)KpAx%vG*rHNh#6a4Z#9kLRhZaL:v)WCJ#TA(-f8Cl?[cYfR#Gj1O#GTY?>"
"?K:;VTeAa7]L>4Y%1ScY#bkB6r8[pC8(U?A*]0J)#R,=`g)@9ocb;UDk6MP'?bJ9]$),##/8o/1Bt:2gQCHP/+AD;$ec:v#cIlv,2dtv#>FOG),@(`$aA]t@Xw*2AJNGDu`h?x%:h)Wu"
"mu?1p)<MZ-$g/W-trc[.hM`o$$qi[%Q2NY5mkLj(ILK&$d$)($Xdd8)+Bg#,',jb3cC9J)su.m'l)bV7i;wH)F9RX&8u+878Xl`>gtWj1HL+F3Xw48&DL-E3?(G-=Uk@1P)h=r9_W431"
"'8<@S``]]cf'2[qPM+n:P7^VYp51lJNpmu7x$8E5]aBAO#8(r9f<[##02:s9qpnY#87e8%c`E)NFZ$H)L/g#55d1v>`@Hv#@.vY#tRgl8u-=;?l;_lJ0cUV$IJ^>#3+eS%aRl'$+J(v#"
"KQk&#E6>##[40U%8pdY5`JQ$$RZ:v#0f'tmKT(9(mS%@5Nw-87ubNe;u'pGNOp8jaC2lY%KUg#&$ZCD3;AN5AQ<;%t//&U#&O,n$LxSkL0aao$xQlf%G:&##9N72gD]qCaO'V'$`xm,$"
"?_WP&/Y(Z#A$^m&`P*&+]lN&+#$930+Bt20O:F>5;S%a3N18E3if,-;]p.m8oGAQ8.I=?>+@A>>3e=v>7h+?>Sot(EN+:)EV+5KCuNwxFufHmJ%pZmJ'V]H)I[H<%H4Is$WuRfL0BWZ#"
">C.w#@(;Z#Tfcn%_VV`3dm`l8oaAVH/5###RY5##FI3T'tv$['Kn.='Xpg5M[Pfj/8`cu@88JC#J?^5/40_o@(<3jLo2h)M3Ab>P)1lqu>`V>jf]%T#[%Z[d>l[]4Nsx02$spI1vV9i0"
"nNHu,ns;kO[,;*#f[S]$Uh1$#3CIgHR%HZ#@Ra5&a^G7&+^r8&`3MC$vp@)$9qw`$3'b=&F)(+$Y&_b$S2H?&kOEH$n0vR&d9ju%NX<T%=uP>#D6'u$9%i;$@qSQ&aF=gLTl8q'9]:Z#"
"dv^^#O=5b#RPj&+@('?5+ZKW$>ks5&2u?s$Hg$)**)7A4fxP`<:%PfCGRns$rc;w$[tU$%?;Rs$GZ0*s9Bu-$V:Y@5<,V#ArkWV$bQ=5pC:th#T^7H#Y2C8.]HNh#=*?u%`uZCuaI.Vm"
"Asd1BYOcYuJOHVQ@v72gNZg.($qJ.UO(Won$iso@o=6hGa0<;$6x<9#=RYa7/=0/16P[&4V-QS7j7;a<]k2#GPQ9u$r])w$Z#+&%E)_'%P`ET%s$2w$Ex$W$`_kgLSX-##1i_v#7C<T%"
"]==,MlQvv,[vUjK7g$8@I+]/j,#1e$bA]t@O0;5(o]YO-9UcO-['(.8sK0N(0JSfLWcgrm0NqRu5n@PSA#82gNfc:HOpcV?$-BMTxZxf.QQUV$<SbS4q)q]7lI<T%LKTU%:u=Y%%6b]%"
"5G'sLEQMS]1%R8%iLSx$2(eS%f%.<$7xH8%97*p%gX,R&DRD@&o$8T.,a9D5Xs^W$OLG12.5n/:#@V>p6eYna.pvC#ZH7g)RB+T%*hVVQ^/YA=:OLS[O8?_[f7^2#ijMsuF3:7%>Mc##"
"=g,X$)g$s$//.w#H`Ov,ujS^3+Q]j%xJk*cR%#Ds$imS%$Pnr?>>Q/U^rc9MV2Lau:Mt$MSI@##tF8$$1oC?#i,hB#SI>F#<NFI#'lsL#?=r$#FOvNFN2,##KFvxFBl-t$`;IK1&aCD3"
"=m5Pu,9N1gq'<[0kcHoIO,:@TO^w@%DUe?Ti+$##?a/@6hr(W-d#^B%1-;hL5D=P]2iL'S`a#XUU#6,%J1['8cdMYY'u92gqbYi9UhelLNgd/;waYPA<ic]F])miKL7R5&bxdg#-`hV$"
"WW9#$s/+*$HgLv#*:h'#sTG+#X9F,28X'(#vT<b$]xgt@qJNP8E^Zp.lB5ou[x,DcHl,h#C'uIu.o0'#wfS=ulZqA+s&xu#ATcQ#L3eKY54HO(1gHY>)@s[$vBio$8axjI$$px=;Gt@t"
"d`Yw0$/m/*sQ.d#`lQg#06vlAruifLE7-##paf;-)1csL-^Sv0TIu@tF^+EraAcrHYf-KCrCDS$hXh/#'WH(#BY%v&n9`)+jY$?$OF5c,#L^X'=[4V17-ET)XE1<1,]jZ#lijM22^K[#"
"n`vV%V&'#,b_sA-L2NZ%E3Yj'poNM(gqe%#2e7VQgSQ%-%=Y%$RkUdM1YT/FH&-Y>:@j;7lhe)MCQ(p$um`i2e6ho@UK.S'=wxP/>;gF4A<Ea4tO[X-;JuTU3=fL$kEl@>Gx2N=anrUC"
"MRMcWi3#'GnfCEBEBGaCtFx>7bFRxn<8_EHpbLIIEn:F*&O-$L;3%@-2JX)>4*0R/;Z0U.:ELwQW1OA#1W3.)QXauA'c%##4-(3vJx:,%J@%%#*O']7sV.&4^[SI2$n729(s]U7j:h-D"
"8,.QJcBBq%>;KQ(R]]Y,)+TQ&ZWo%#m$1v-?k^M'nGVo&)E1*sr+`s@lED<#SZAR.Fa3@5/wha3EUr8.bTVt6F8+l(N77<.dN1K(-=oT%'LB;QRGiaYhb3A9Aq>X<G74lmPgoP=-G7(H"
"h9Um#jm3`;k$u9/pIi?u6.obuMMGSdq>^Cu]#Cre81LcDW3q+rk`SV-QMUv7sPV@.t3vr-@mkU.Vc%&4b6:(4*&?D<$f`E<AHvX?F.$eDhMH6Jd)V-)A[6:7vv&Z-3E5p%8^4G.9_CdM"
":+kKpB/Z9.YQa#/Ma^5/BLd4(1/jb=^)BO3Wxpd4x?.J3x1%HYe5SD:@^LJ1-^?w;=5=3?-e^sW+m.70&oDN9hGR6:2=+N#Oa3G4Rf36:q^kxiH>7.26/5##vCDS$)Z(*%6TG[72(,J2"
"<XXG<9uv1:`SkGD^'Xq%IwXJ(:sET'NfFZ,0_LK(Sqe%#t9>d$0aHV%UAv^]XN3*s0@A(#W5Ys-[.3:8(O,<.7gDb3X%6=MAk*F3fgt_&R9iP8I9[>DoDc:9,WLE?qp&[`#&$nUCsSp<"
"'DD0<k`QJ(+JZO;l2MB#Z.uu#iCtbjx7i):X-h)<#crS$Yr60#Z3=&#$kv;/:P`H2VBQ<6)WB,;I>su?VX2+EvcdUI,s`P'IV5#-KP5j)DqE9%1Igm&caf%#Yc.X-o-9^-b%b]+(/%g;"
"P_+5pfbGL%>YHs-A-hM:i@Ba<%li?#xsGJ(CqC.3JCkj1p:Rv$6ZV,u@RBf3dWmCn/);dc$M)sucvDKr`dtQ#Cx8a]_pa.DN7IL2K=Z,03?-C#LF7@#Q=(Fr7T,F]C]3r$b$kt$<DXi%"
"ju4Q17mJM)KE>##&jFj)%u9t6N?^2'uFBT7F<cj'B@RS%if%L(U8%kKvT2'OKjq3Ac.&CuZ]J0Y,8t,#(]/Cu3+Jf$Pa7H#pR8e%XWYV-8*x9.kDsI3K)'J30bFx#n/42C1,#J#;V0Z`"
"O@i@`L*-[$_$0FEr873uqBe$VGDY-'-/5##AbnD$<r60#wD-(#^9h_6&KI-3S[jq8,(<I;]EY0;c0ho@Xw2+EUFC)E+X<#G&Q6Z$>p#o/7+]8%#7g&$`IZj)@(v>#+XUj([v;a*]x:%-"
"twx_#]dweMGuWKp1Ndc(Li^pT8$bs>>*6k([]Bj2;W*3(Z&@?];[kD#b_*<8t$s:8V+uGMkm$cr-m#>&4@BwA[q=^,U3ZGd=c6w7u%16M;ibY`wP=pTF5PQ=,[mUmIu+G*Y;?l1&CHx8"
"h9ME#2h[;Q--hK<JPM'u*S3'.?=1]`%9%##3*c6#7M''%MR@%#kxV#$.]RXHXWl3'/,?=$@L82'Gqe%#YVvc*X_>/(39eaNnJR^$1LD<#s9k>74LQlLV@_;.&P-98-;kM(AM@T%(_O=$"
"jLZb*3hUk;=w&[`QmA<^X%gp;$^S60g(NCa_?C4fh(]0f%/5##9upb%p5GDj-l68%]EFa,U?ZS7uRN`#6+.W$`7?\?#u@]lAM1blAXg_V$fN0*sH0hUmQ#.87fE36pSL;8.ghgfLQlp+M"
"@>>H`lSc_bOL+p@=])]b=Rtc)?TC*+2.l22S^TY$f%5^%[iU,+37$U7HLeL*plcB&:ValA_+XlAmOT'+s^3&+ewEtC++ikL:E^t@drk_.],%12$oxF-tH^.>Eo#<.;Qrc)v2u?dNB%=1"
"+LMT1E$E+*lii%X.pUm8vfOD(`*2$,c6>BGE4KtU;MQ%b,k68[HnNP&2Q`,='JBU#jN3p%`b:w,cI,v,8Z,,2V9VT7Lo^dDW[wl&+DY>#b2Cw-=ux5>g#<Q/1GG>#=:iZ#^@/A&9()$#"
"^+S5'%JeS%$h6&O4gsWuLq,F3hx^t@UGCs-R';J<Y%vcaSTw<>SfkAuJbko@-SP&>r/pA$aRWo7o2fn#(_M@t,DKfuqAM$um5_Buw.>>#f76I$D2B'])<SY,xlP>#[g/e$gMT2#j]K>#"
"lgT/(]VC7nb@&Q/Vl*F3kH([`821QA3*c6#E<b=$v'U'#d?.'$1%H0)sf(q8_bxv$K-&&,Wahs/la^Y$XDQV%[u#v,C]6X7>pQ'Gps;A+fLQq.thpB+1_)D+%?I2(Y>e<(nc]t@X6Q4("
"O`9D5t9'3A$,@?6p'oA6>#sHMobw>8O(=^,7tm68U#-g)vp[GMriL:%oh0e)10nL<nIX;$C@-E3ln6O`K$;&C$MT^G_CLK[q^J0NG$qH<(>*a,*4flJF,&n0(U^(ukoJvJu`CH>,#WRd"
"4b?gLL'rm($%OW8veIc87B3UMJ6v8#:PX*vRtNrL=#lU&#Dfr6.6fS'ZBsL)#TKU'%NYN)FtYj'/k-d)&+pm&=k?5+.b;)+YDj1+Z2hu$-nhsf;H#6@d<O2(V:w/:#8hWJTv^>$i?hio"
"PW;W#k-MW#@qBcST/^(T@+uf1+8`=csxQJ(eR&[6*8E.q)Hh#j*`L'M@=dmuI-juL]I'%#&7X[7quE>5#/hS8q(>78i7h-D82ImJxa`^mtn,Z,(%KQ&8ke`27o0N(D-a^moHc.<pV$BC"
"d+hN`Q/g<Es@hUOIHI=;#ppQUEdefNDWd-=G'0J1,f]D7FT3pu?dcc9[?xb.?<QfL?TxqLp+p/NRhd/;*=:&$r;((,xj)($@lE*,dd(;(E42Z#;XvU7BLsl&<ZGp%q3J=-?gdm'%>0N("
"s'uV%p'5DZ;Xkv=kGU&C^<St&Wr[[IA?p7OWIPkuDn^.;LddC#@^RC;bhbT/hZ-o#d[K(kqF7*:Lr7T%rUHG$=;r$#hVQ&#nnU<-Hq/q%6G:;$/u3X$4;###QRn$GL[N9%7uS:)^Cr8."
"3'oA6m_.W-r4,h<=OLp7SACW-q,^YG2h[I4%x$O`UO]i?Zx[$$j1Od;I_%T$w1U5;,@S'JO5++v=e5R$$XH(#OlD'%_ITf)*j#8/W?h,3$]q88P4l8ADZl_=t:k'>6c$aEt9@2CS7dL)"
"3wvW(Yt]#0soQ##w1[?$3-Kv-TcM>$%Vts6$#V50*`,_+dVh:%2bX1;K&JTAhf<]$86)c=06/pClV8f3qwkj1qjWYG)6Be=+7L+*1rSfL9gn9*-br_jkAc<&8u<wJiMNdE@qT=RJ)))5"
"oW*$f3<C<fR9qx0b^aOLa3:q9psl2cC:pg8[r8>83mHr%^9>TS,2AT23.F=.fH#L<RYr;.h2d7&*jr*#cFs%viOUr$2sn%#9[Qq&sj8j0FT:0(+;rQ&itL`+0E5p%,+iK,k?ZxOfpL0("
"I<'E<^4HZ#4M/2(11>.3=j.4(i>=6Lh(#K3^+)X-GWm$.D&+P(k;oO(H16g)A4qw%9I+`#Ztwp2+f-wKe>*hu+?T+*R[j3C[QPN'g*Cp#VSpo7*qsjuc]=t0?2er%ln.F/fDDS$#9Ss9"
"h3:V&o=[5J4fhV$WKuJ(vxC?#[VNA+Q-YaurhKB+dW3o8%ekAu)/i>7)&)p9,Ll)4x,Tv-(5L-'?r*F3EIDeA[v>5c%<D`+nFx^ZHD69;tQrp#&@<L>roJq/E=D581P@Z6QcriKjPT#%"
"'%Wv7'^;w$Gixx$ht_$%;la>5j4D&==;1W$DJu?0$=bx#Ps9'#0)w(#R@p*#9MhJ#PI>^=saZp.4lLZ#5W]._>-4H+*b_r&erq4J_'v=PC42?#?eww#LZ:v#o8Z?&*FZs0OlSc=OL,HN"
"9jdo@T]`Rh8[tiKqI3nu5lor?26-p@$u/p@$muoI1aTb%p'&##l',p@ckB`N6,Dw#q>$C#&<?$$xAZZ$0M+U&Cco]$dnU_$-_[D$9NgY&NHbE$Xc2%&^+s[&s+wg#l4s`%%PJA&(h%p&"
"59iX/On7W$B?(0(GrTgL9`Pt$(2sw$nOW]'9H3T'Na>##u8Q0(,1EpLsMff$=V^gLJN?65lcdC#rud_u;$sXl^7Rlu<T:T#JcbY,MrL1.Y#`FMYjs0$]Is-$&'(?6Rl7/LpmLd;/Bph7"
"qvf^+O&`G)vaEQ/KsxG;_sil8^p.m8kl^,;3wgs?,e's?4'Ls?QRV^FFckcDT[2^Fd8j^+<rQS%hICZ%qlM`%V5v7&8cF$gsne%#KvVA+1Sc>#=haT%<.@s$`vf*%P]:Z#>nwjkfaJ<$"
"?m]._@?k),$+gx$]wu=PMR`?#l%J5'2S;<'3fL;$bV_$'eUv23f%Ne$cmeO(UTs529%s=-(F+c%-ueC#?6@su.pPXj(j^%=N@WD#/NKf:$pYx=/jPG2$p)AXCaAkF=b$JC%=vjtWhd##"
"xt_A$_c1a<U_D#G:.rv#K9Xx#17gE#s?D,#Mrm,$O>&B+g(C]$HfU;$bCcB$*GuY#:U&m&+M1v#0K-HFuYdj'4]U;$24trLS5Hb%U;CT.M@&(#4Gi*%=kcr9]Nvq=hAo>.tU=u%Pt6Ku"
"1f:pu6Psu>5?-p@NW)MgfwE=c:qKfLwms,#7t](M[h`s%IaAa5(`/&$Xkl'$%-f)$=sg^%toM`%>(`v#of1Z#mesD#04#-2<eiS&-;ZZ&(R(_&dJ:C&g5#3(6`9D50Dv<-`+f'&;oT:u"
")cNLue6]Hun(<nO*3N9NE:k9MqiHoIE_'H)ND[9M)8x12wDBg:aHW/D:9_v$7@0F,humr%e>;E<Ck86&F@Do(E,wF#QeV$#f/5##/H=#-%GBT7^BO,2#gU@6gJ%12Ft6B4F'BD588JC#"
"r-Tv-gc``3U5%&4AC7lLw_U%6)jTv-[5MG)dqh[,at@T%U(XD#vb-a]3FTV33IpnkiiUlNL^B4O4*rhGu;7]`j'4CU4LrV`+]/0PS/eq4g'OqOa]usL,@%5Jp[$m/J(-&$8)Xp'6<%`$"
")8G>#GSMG<U*+,2X*387>9#@0<3xl/MnHoI)@k34-$F$M#I+RMV2e(>v*;j(HIvk(cNSm(nn_,)sQt@7jXW^#<iT^+F.=5&Nvf^+6@wo%h*Dw,6iu##_V>;-Q4vghuNW2(F[BOFW*SXq"
"6T@l1qApS%'i0Xq&^7`o6p'W-F%X`oWp7]`QNQ[M-<^E#3<96$75=&#WTw#%>hjp%rbY(+bced)TwUx5nLM.;7Z;u?5q4?>PPs:@jFZAF&QqXH+/MXH(lP$,3w($>(t7x,GQw61m2-/("
"MR<E2HD*,2VV>W-15Cq`Ku^.=td37%RbtD#%)<T%bQCD3s?L]$MeaN1AZ_?Axu8_@wAT$&xZE31AQu#AnM+oC,k@*$?xljad?WM<LO^>[W+ew;)`FODUHLSY6+LcDrP2<%Vpd(#STBb%"
"Bn;YPt.HVH;Jp2)2G.I2K@R$Gv9JX$D^:O'Vv9B#l)l/44rIR3fE3?#SiSY,l>[1gFfi?-K':j*Ve,c,$e)L+r2Oa*GTxg:/0?X?`&6UIR:rS%NdF#-]/7T%K&5;-(*^I$A*bX$ZQ&*%"
"m-7)*9Lu2v-Ei:'$-9w#?FoA#6)@v$>MP##`?tx#Ddq,)8@/b<OfUw#Gn6U7YeD^FG?cj'VFNp%A2(KDUCT;.]1DeP-ShJDdgW/2HiVh#qilA6>$<F*d7rp.GLs&3UNSwS8P7C#d6;p@"
")pBm;&FvX3R5rq%[3>sgvauaRO%.W?,0QGR3`S81`6ScG`3ScGVp4uul7vju5kT<1&>1Pb(I`NW)LH_>b8Qc>MD^eq$`w@4B6]uGruoM9V8jo7gAZ,F&q&j0J2vk0KA@m0BltU&sKha#"
"7k)q0ojr`-Ba.B7hGus02J`A/43eR&VtiZ#8Tje3oQ^u$UBF^69H20(0O1r&f%tE*g?(0(X*sv#v)9;.Y-c'#*RxQ8@R&'-n*9?7t4g^$P8qJD/TA-uIR<))x4_t@rNhV06uAgLW(rW$"
"Z6ho@_=@8%<V*^,po8V-1#4v%cXmUAO2-g)dvL8I*Fkc4.v#e4?+MB+K,u+`,R(W1)[[23IkoY#1j+ID_4F%$vl;F3BDo`'MF($,`1FL1N7CGDaO[p.A:o:dprLJUepGA4KF$##tdv=P"
"cPGVH;BcQ#?U%[#xsji0X?x'/5o4>%_Y4?-vpkv-d.]7N7sZ7&3g4(/Fm#YlWU@a<tuK,3hTp$.vEJvL'6xU7aI)20KJX:0nu)Z#*UYm''95s@R0QD-TLL@-xPL@-YH2&.D'nmM@N?/("
"wJ9W7%C0?$gaE^,/NHQ:W+E:/rCDS$UpDuLnbamL9kA%#mkQfLMK6##)&78/:XKG.Ko3&N+j4c4tX;H.,PE%N)ZSf3uD/Z$ih]F-6WBd3E/?x#kw/J5a,2>5q>WA5K=I##6j$]-dUDwg"
"DNs`=/%,/i^c`A.0jqr.UOh;%T[m50(N*##Bq%Z#-=it:1F.lLq[+.#=XR)$=U_/#rk(b7m$DJ<50hB#Jmls.$4u/2Ji=>%;9[P1(*@F#NXYv.e,k22KkZx.r2^$/98;&/giJ^+&vUT/"
"`.&?$:oX%5;u%H+W2iK(-O5n&t=Ew$45Q8/ovql1sTCL56?g8.+&+22C8C78xq7jiT%NT/($8_+8_KF,vU*0RH8at%0^PLDEu=/('Qi`-I0Nw,?]$),7CDn&6LZ+<)1<L)__QM3g[Q:v"
"u1V7np[>#PNQw.:D;c;-*T8c$FI:j(FdoC%9XgS8e*(Z-wD>Z-lfsw9GI1j(nK0_-MaaRExs*F.ZWw8'1MTpAL55##p5([-%+'_S=](##2j9_S3d,_SBev=P)Sj+MC>-##s.p&?8-1lo"
"sV[YQ8k#T8oF-)/H2PuuDgV[%_E=#P3:35&pM?Z.lS?,$s%<)%-J(v#9@3p%8ll##_IZ?#tLiG%tv+b%k(%`&lkv23AT3u@l@,87I9I12.8bi2^a7H#v1:8.:bM8.P=B;6m`*GR4@)I`"
"LN.@#$HSX#gJdG$.?uu#p.:kFvl?#>OQ4K1ZC@Z7To?8%xgf>#?[Ep%q?:lK(J:;$_Gp>#aOd?#vX.d%v,G'&m4@%'%MDIHu$Ns-^q$KFEBp;.it#gL2Bi-MVW*h`3V:s`:/T*I=>#rb"
"4;NjLvrQ:v2R)##5U/^=WMl%=:E-)*C#Sp7GK5W.Invq7]bUAl+:?#P;5TD=#KDAP=3Os-G4Cp7,(5G.LQKI.oeQF.]cl[PmcCDjq-c]P^FwK)NeXG2Z+e]#t8j+NVBk/4B5E,EH_+8M"
"?OPa8bJ5s.$lor[@c1#?17R>?t.DPpp&h)+:0wEN,)>Z-g3AF.*NHr%k>>Z-FsIF.ZYc[Pb#GwMLWlLu$@dU7+H'(,bd2-,C#sx+$S3N9bx?>#gs-DNGe'H)kc.;6Eqt5&3MC;$4k'/%"
"<@nS%`b:w,@SSiB8L<T%=(%s$a9-jBEN(v#/_0JpkP)tmDI`22v-MkLsPRL2*B4:.vI8]$,t.p.Dcp@F[uTYuR6/vLNc#8R3Gpxt,:,f$9>DYPDGT##kD,MT]BAG`cu:eHL(el/Jt+u'"
"'whv'r9.d)7r+j'9c&m&`qbf(q[U6pBdwl$k;v2AA39K)?]d8/I9f;-KC@-+[)_#$r,%PYI6@lYH1ptCRt7xBqgiS@FMYVYFJG;Y]TFuC$RIuLX#R7Vtk+mA^n##,m:7PJT>X;&*/ww-"
"=@YC-6^sG#JAlI#i:7K#CJh,-=>d?,PIL97EA_[#n%bE**D7w#]rb;-wJFV7wXYc*9]Mv#FG-s.Y1PV-'F<?#U>ro&QIDv#*bX,2?)b@5%CO;p6RQp:F=St@2R-@5[FhI3EGKO(_AN%."
"i`2K((<('#OD]`*gmlT%I-]]4%n:O'dL0+*.N$t]:2:ru_:Z&kScW-82I92H2r$V&t.*7WSc5/2o42O#l5>;-=7ri0GvRduiOT$-w?Yx%9=8?>mrF58(#wE^L+,##[H:;$M%E<#Q4,a7"
":`^c35/)#$xai[,a/FT'K:b2*`lqf#t#T^+@a8q'L&(E<FBu/(Ca@[%4flY#AQ^S%B/L;._fn`*l6JW$WqNt$TBo<$0rFS7%)###SXc?Id5<I3H'$@5i;EDu=w26.b9EU8M3e,vQbj=."
"g=j<HHfEHt/p=j)60fX-Ynn8%>3e]OQ<Q8DPoH4fpw)<&2>o+PN6vjugZJ*$lYef`I`CA4fbtr-^AnK26OY@k%r+9`hkb?Kh<q:ZdkYGD[SJP8.As=YBD2W$n>cX$q0kA-HL@7(^2,03"
"hQt8(qiU13dhk:(NkO33@?mPA'(;TA.(m<@Zl3KE2b[MChDh8Bh2RmJGIX?H0dYm<g(q`*XBZ:0Jr07'UCw=7w)7L(>]e3'_wA2'e+GE$C$t_53n060Q06M(5b&<$V]>>#>kJJ+Bk$qr"
"&$<F*S`+12Y>^;-7oCp.X(XGu*$`SM46vkKb<)s@*`0jKT0D*4(xkj1C4NT/s@bAf=@kJ%i[v;-O%EU%`Km(N&Th[-n^5U9pAGZ*B76u6]#b'I-]K>8AG:+K3M)?&p6?4NCso60/d$X="
"`v.UC;hTZ7lOq+Y+NJi`F%h%1YN-F@B_<MsFKfA,6$1U/m?at6Ajt#-.A^71V9SN)+W<2E2@Hg1<;9IO4]Dc+]gv,3Utbu77R?K)&;iRLpeC>%;(D/=GQl_5:W>r+LJMG5iB9r.ZUYL<"
"0>>s7C4jK*r,hQ&-->K124il2vWrF*qYaa*q`$(#3ZxI_BeX`W=RNP&FsL3$/>####BH0(&.&(#M7EU8QlQ$$/&Y^Tf#Y^Ts_+WMXn5[jsp2H2Ew:K$Lv<9#Vu_Y7Ygi2B=boi'(eaD<"
"6+Rs$43=w#p=+m'WMZ?$bR0n'K?P3'-peY/&#P12X#.872x%t@sb(j2**=8.xkYcD:`V]-:()vh08<Y#Dt1Mg#J$g_#[<G`i0off[?I`%-4:on.V9Z,rJf%F4g6g1Rpw0)%p02)SJ,G4"
"L2_H4uoFb-O?na3h85j)@s5x#_7w8%x>5p%/l1`+FvDG)Wjpu$W:G_+n`HI21Ts:Ql2q7/?22*so[vc(r:=3pFPgk($H=<#nh;1&MR7W-HovRVYt]:/*K=r7G/oP'm9W58>`,g)r_/T%"
"m''iu;+J$H4j'nB5q;c-U$]pc:5w[,cKaD5Oi=N1OJxA6(TUjm1e.:a1&spunv#'Xdsii2uK+IX]GpVLpVTA,YVZW.pUB@b>0Wv^0^hxZSv7N-1*i:#r:;0vjYta$$tYl$33U]4(]oAH"
":'[W(.'<P*S:j]/6hq2'-h<^,17-K)S9]9.@;211A^'V.*bqC4Eo?l9qt`FE`:Y0C#U3+E/7d(+P6sd*l%es.Z2lb+%d32(Z1*#$h5JWZF5###ggJm/J^rh2cL*E,rH@_YoKgF,Pl:v#"
"8^XZ#;FSeOr<<E2/Y)E66KW6pgA,LM$.<E2CR1V6v-Yt(>cH?uF9JC#4_7H#uaa>-iJ^e$1&7`&rj'B#.tYo&/[^:/;0S`ErY2<%2<YYufNYS.&p+Y#2Wuo.1lGf:qL:L#I(Mi^355L#"
"e`]qkvDw6X5Su>JJDADG9XOY-S%4uRXZnG,B3juPb2Vt:a*VJ`,]k'8;'UE#r3biBVD98R+gS5BvUrB/:2bl(_kkReMth=6h+'p7GD,H3hDZ,FN&:T.K]?D.$X2B%.rkF3-3eH3D#>r7"
"OU3q8hTC%>pl<'>V?8C%:FE+E5P5H*6=s@#T)0e*ok%m,RI,^4eK_G3fDI12**-s.IC%p.:d;`-p7HY-PG6U.5x^t@qi4p7Js=5p-H1v7r>eZ$x,Tv-j'/oBi=^G3lMkA55$s4VM=;%t"
"jU)o?T)uB4T)p:VBfwRUg)b)>AcY'?SUoNo8<OG2d[[Q9nRO-[>([Y#7&iJ(G';2gGj$)*,sC^-v1Ed$v9qoIG*#n&8]U;$N)Rd)kfe-)C@K2'G;/_4V?g=cTZGb%+qwl/RbMm/OP]A("
"w%Y>)p/hUmMxO12hUV'#%XeJ3`=Y@57//(#a>0N(xE3L#:o0N(PHCD3lH+U%o_Y)4dE90Ke>cqpgHA^aq+Xu#qs-s#-PI[#uGsD#)D,j#g><L#v7eL^;sN/#C8J>,Z[N]bx0v4%>`RY>"
"U-]rH/2%>+2,:8.QjhnucV)0%:o/M#G9P>#H4Ox#PD22%(,Qj)q-TE#9g_V$Gnr?#=HhG)p$F1so3<E2HD*,2LD0V6iZqG2_DsI3upe4dk`b4ds=#rbTIZc%Z?/=#4;DK$4C%%#VR#2*"
"M'gQ&9lCv#Uw7@#`a<h#XW`k#1U(Z#_U/m&7fY>#>d*A%Hm>^=QK,R&//Pj)Av`hL(A]8%IBlf(bXa*.cqJ2'mQX,2;^qJ1qr$s$hoJ128>Y@5^;1I$,[FpL?@aI3^(+22QdJ,3TZ>g$"
")m)NN1%9;-9C*j0Xdko@l*M8.XF3]-F9Xs6:]5g)nv,Z$m>@XD&po:?QLbFuknNmD(t]uYFDNc#hJ=eum227U#SvN#k(cAUm]+##3xL$#GL6c$1n'hL+H*#JeEFv#SQ0>$rSd?$?x4#%"
"lqL_$+Tun(V<xw#EU.W$vY-mJEK10([#ic)P/[S%s[j)#3Ufi'=xF/C,hV1)$Z:6&#Bd&4sTWv#mKaV$QilA6,0pJpU7,D1rpPC#eUv23Uv[@5#[g^$;QQ=(=c*j.V<bB#L;iP-2TcO-"
"D<YD3[D_#$:1I%=fWbgf4Y`2'$wg1p#VO&uAo6t#E^xjIW4&NKNiVV?tt(##D1U+vx724%6(+&#/pm(#<qlh#;kA6&^;`k'puEM(l*;w,.1Lf)@(&#>83:W?K#n9@HKU@>LCqK)R)w`*"
"n4N8&w.Kb*.Vss%G9J,$5CcB&9[C$'7-Wl*7T8M+AeGO0Mc3@$b-Dr&d21>$s21)3Ar0HMVRom0&JJU)-b]uGP]ED#o8NA5qBfP/=ju-5-7Ap.j<Y@5R$8I$xT<8pp=Y@57m_a38D9H#"
"B]e[#]::8.DlE<%,T_^#El?D%w,/J3O08@#dk%>-I,6p/$`^F*P[NT/:eTk$_)vD#k0&b?;Lu0Ha(+XM$M;xA3bbV$IWpU=X_>_C:q-J50&2x]&F4'6NwtD6U^?F64A.u^>9`a#LtZ@k"
"5uFe5g2>5/$t3'c<S/T%O1x[d%^edue&ml18=UO;`oG1l`gupO>W;l8q@gO;Ww*##sC24#+FFE$bYI%#)dZ(#/dT`$2wrh)fR+70)aO$6X@s.;V.]Y>H/EQAE-p'=Um_@>&5$&Ff3neD"
";we2CN:Me*,B>=[M)`k'ek;<[h:+l'[0_S2w@M'#x_;$#<QWP0esr4(RbN<mbc45(H=%,2oSZ^3,a9D5Z5gg2^a7H#%cE#H$g0^#E54_$h]DD3En4g1<#B_%jHuD#%>PjLgwre$n]E.3"
"at9mrGdcI9@l8SaRCD>NlVCRhPLR:u9H:*BA/SEIj&wx--P+0Li+Et:oN2=6)vkU%A+?#qD$x,R`rti'Kh%m&&ST)StH4gHarqsMn/8H5B$r3X%T$1D6+^MXQjR+i<o>2^C&mi''>s80"
"BYcr'5Z6>5;sN]%B/Tm&PeFG22^K[#Y?u/LiEfN09#H+<&+3Krv>[4Ae*^#AP&_)2uPI/3?R1V6viLZ-d9V-Mno`]$*a92^`@0#,$67#,?kf87s;,AX884AuP.n7@$##YuSr@/LJYur$"
")tj%=HOj;$tsp)&Ze320@79;61^c#>1>>##KUFG2(?tZ#?k/6&.krR8fx%I*mnu,#9-%12%%ht@(aDb&3$$&4CL;?#fQ%[$x7jH$-iJ.$PMn208hNul<NT+_<Z8L#Y7*4#2Fsl&33d]P"
"S)0a=GWtR8q2;9/0;rD3>HH,M.,/W$VpnI-Mt>o*`0cjTE_<VMb*5ltqD:/u`7Z9MFb)g(M,mQ#[t[t^Yv8uGclw20;YO.#)LFi$Z0`$#7YcJ2nAf68:kO$>W(uKCu)GwGsTt6&WsKQ&"
"G99I$dQu2vFke$'/E,F%:[rkL`nWS9Nt@$G<]1Z#t`_Q&?+V$#?L@w#S8@h(Y+2v#9&Fa$;_3X$V7Zv&Y@eDaeWk,2wU1@mYo<.3O9et@T_Bdkwgji0+`rs@d$eX-8*x9.qQx*4bQ<?#"
"d[To7&.HuPEl`uLmW=`6FQGF#G^[8ux&ehuZ_/SOmnYFu&L^2#7B:nuIZtE$@M7%#oCxV%IdYR&*eY^$>X&6&E3:,)<eqS*&'UN*Ul8B+1)N1(]+1v@`T:Y5PP$WA1q-B5tc_;.Kn)b$"
"$9kG3jPR^$PP.F3H36H&<.3d>_T&[`BgG#Ahb`0I5BqZ7jD&&tT)DE=;.dS7fmNoR,*,pI:)tr-d?q.C/kn_+?u[?$HLG:&jW.<&7,C>&^<))-j]TV0&?Je,CC8q1ReXr1rwd5/+r3t$"
"2oBf)Xk8/17^AB+;'.k(ZRv##ji$r%MxO12nTbd(p@&(#d]`I+_8*22YR4f,/;dt$^D=a*-[xT.p&PA#hafK'>D)pI:Ik@eJ,?X#JCD5VkvUfUfxG<<T9+,B8-+T06RurS)4&VN/^;.."
"'r>290iQaJwpeRe_q+d3)mUG+>K@d4W>T*FoR$puYhiR^gQ8QbSlQ+#5mX+`p3tV?[]ql&xc_/E>$0U%WQg.$46%h&3`1?#9&;)3qElp@LMamA;`uY#?eNT%7hU;$F9TQ&3R1v#DRq#$"
"DM<5&-s?W%hC@<$?LrZ#C*96&LAwo%/YL;$>esP&iQNv#eZ3$#nwD8p_j)22hZ4N3IfED#b:YDNk/RL2#@V>pI47DN[g'c%EHw@''0wGG2nPu%:H7g)[XR@#I*Qft'<mNuPTx-u1P3Lu"
"ToKv`.@;k#[tPPu/[_:H?nl>Qe?rxtlVs,2Tu_Y7W.@W$l-l:&kGQiKFKhG)C/4J#Q<P>#JsqG)9&US%Y(N`%fJ]t@:&2F32u%t@GL0V6Q,'j2-k(]$Ho0N(Lc.Z$XtoRui`$m$^1JDt"
"W3,DMZ#U5#@x'/#n35B$6g1$#*pSIMPhi;&Fwe%#1=x#/]JoA(e6GF@WYp2S`TB#e-4JM`q306$QZIjMh*?wL8jCEOfbC;QiRPV-RXMg*h(kS7d#7L(=SR3'B/L;.5.`$#p_^'+a#vC3"
"A<?w$eL*C&^1-k1qCn;%d(4I)UxUO&#M,G4*-+c4*^B.*.DRD&X$VML_BVnD^e:o22FC(.wUbLD?XglCi2k]%H:'vRNah;FYwj5]0@$?$EnCrL4*d+#h)0PSAQIS[<598.Urdu>Ya:6&"
"o%er%<Cgt%=P0;0qY><0;f%>0^RZp.c4%=-&OZc*DDQ>#X@G]%'0Xq.x_WS&#MM'#)E&7pFMeCuWjTx&/',k-^a690T.fo@.4u7'J81>GL7?g)U:@P339M($w5^f2m(tR:SuQm;dq*c."
"CIVP2U4_Q'0wLki4?gp^CHB@@YDZh#:]vq.`5J[Yi.sb.)HDg(iOgh(nrA<ZDtc=V;pJU$LvKe;C2NX8*B+sI?`5>m*><T%,['B-90Oa$/69Y&[OlKCGs(_=AA`_=cN]N0e),##f-sr$"
"WIj5/(xD8p)]fn9i(:<fY=a$>-0<Q/3Ygkd5,+;b$dcbI%29CJ(3h>#c%GY#NFlV#kbsko+IllY'8>5#aC6T@?Zqr$:0W<#Bam^7-rjp%n=9B+<m#01B/ZG2HFFb3q,]M9u3`1C0-B,E"
"@9Kr.S^H)*.8v]#B`wC##i^j(e:=I)V#VG)UDLv-1GTF*wZNFp[qP'c@2^7pT%720G<,DuIq'*)Qn$b[1)f`*$.E<-tKff$]RG)4Lf`^#JG=_$bdf[#g`Hv$*]vb8<no'50><xs;JPnK"
"a]rO#[GQU#OZ5lL4q8jG[.Tl]+$rv?Id(3G[*oUdLW5D5rD@lJRcdG#9Y4)*$d)]X.^mourq90#9.1GDM8s872H6>5d(2A=L)#NMpbG?,-#@?$5`&A$-pG]64E@B$O1A_6[Lr:'Mr#5;"
",1pg4R@)+*]5@*=^e1UJ>uWnJV1S#H$3SrJmOpE#44?]8hc=R8L'fe*0F(A$8)C<7a2dg$^fUa<u[HQ8kEFl9#DgG3(WAq'5MXq&.pg%#^Np%#FGZ)>7Yq%#';$8(BGv2AOE(K1hl^t@"
"`=Y@5E5+@5qi/V:ZmH12:M^-M@64N39LI6qnIYc'a%NT/j&W:-c]DD3Tp_20GF;8.U%`v#k;vPhBEqJ%jg,Q6t#Vi>bMus&[mZLX$vVU@GpL/H`kiUCJeM,Wi0#'Gh#k/@F65aCkk189"
"AF8tt$50?-2RokMA;mV)`x]U=j0')I$w:,2GOrL2PAuteQBv6DrXMV.*fk&K*7,',Oe(E>7K5O0<mg6/A(uOD^AR77pBGT/r/#iu>2%:;XrWI)8GN^@'6>##F,qF$sM0:#[?O&#]:5a7"
";1Yj'.Kc40f^Z`*$I>GMfr@H)&tl]6@w0h;`-l<%RvBr.Ts[u6YiWe)=Un<$BKP5&SLO^,jqno%KcEC4Bu`Y1VR]M1,65V1m7ao&1g1*svNwH;l?as@5b7H#-`1C?)).e%f<_=%@PWj1"
"E0pG;^LSj0+S.on.C,/6du4>;'h%4p_0i@H$gu*>hhUmBn-'/.ZUF?.V[BW/X0O,[B'-T%Z8>^cTE;bluA.@#]:V>Zko*xjclIegP2/D#)rKQ4kkq],T15##%A28$dw<9#F9F&#,8rq'"
"3a:^#@a=^,[ESM)l*w8'lWWp'3jFa$NrHc$lE)L+&fo&&-YW9)NQt9%9B%)$-BA&,k1`Z#e$G/(KMs6*:f:<-EvD&+vj'-2&Uet@(q]f$(*387mh-=7:0-@5+X+:8.I#<.WYNO',VX8/"
"b'Fs-9#+xH[5]v5X0$;d]Ihw=vUD<t&l:PUK@mlUft(9TXY7S5YKD.hgKxo/DrA?S._n<0J1am-XpSqJ?ms@8J5YY#L=g$(Scc##/sf[$D:;?#^()j0AS:[#IGlY#DJ7I%sic;-b4`]0"
"8DeC#lc&K&Lmr=MAP=@$[),##4U]]$&f]=#<^fW7=dVa-@UXKl@n*?#LO(?%;goW7E';'#d8I12)l]#ATDN)3x%.<.VF3]-+tAotcqmU5j9QwLTD,-N]LfVd.fZS%/io51$V]=%=(ED%"
"=U<T%B.vY#8OWp%aI8W?K`1<$e?Z*4)J:;$.sTs$<:eS%>pek(^KW6p;S+XC=I3IN/e7ja$eH?uI[bo@(<3jL%$qc-RebVdl:tdur#p%O::d(j7%?@F;O;1uQci,#VXp6ve`UV$>)U'#"
"'3'@%u>cS/>>;h1s^m)4ZQ%U70h#X?ob/n&I_*9%JZbY->%Gm'IMI_#4K]A,isH)*LEbT%u=)Z-^Y###EXs^#^A(M:HWlf(BUZgLU<:D5LLk$1`j)22GQQ=(d68RuSmJg2^a7H#MkAp."
"jU5g).*ij;ST)*4;`Up$^eY)4Dc'-uj;pe$hLI]GhqZE.9Ss`uRKKV@DiTd@0Te:&Nq6nK_Q4nie,.V@;8+0?;$2]$=ia:IfThhuO`Fs3=tXp%%Nu0LA7wYOA]7V3?-YT%ojL+r6X*mS"
";L#&+oAf%FcEF?#B'#N'd7^^+wa<m/cI#E$U6;d,j'X[&>NH^&V'1R/Aa6G,;dk-$.*FM0ifQ3';3A##X9K$$v]c40*uN70*`O6'_G6S/ZsHT6bmS^3pC*,2UQ&7pWrcW-@h=(&`xgt@"
"V::8.Yr90k`/E.3tk0Xq6r@sZ5(7O^)Uj15DdZ0;FEoT*Mh%bP4NgU1%9IhU,?G@:?B#3L`vTP)*Dr#VX`E>.v.i*>UBEIbACFe3CK=Z^;x7L2GP(Ku=<h@;*/4IFth$&;v)$rBJ05##"
"ZPUV$Cw<9#,GY##,C54:rswY#G5qb$H5aA+,l6s$A>cY#B5a%$MKX*$o]H,$3iqV$.lQS%,JcY#,9;G#*,EQ7I)-,2XSBI*qlSL2;9J+MS,q>$B`7H#VYH>#TxhV$SRPj#``1?#CUYbu"
"FK%WtwK1jtQ8+_JP(b(Wr,,sH6U/2'xBv5(qU+#'vsZs&NWv,M`4?/_p:Y#M4klgLQRId+&oB#$7cU>%B:%<$*2G>#:$gQ&tm*?#igk,2J0^p(KFrkL(Gp;-(u1T.86RL2F#9;-7J?+Y"
"<VQX$(]@5J$CF/($=CJCMtQCj-9*YPX^cR#k:h(W#mVsuWsf(aU3]6c,k`CjmGvuZ?esiLtXm#?H[3-$+4)rVnWYqV+9,F%wo8iLY7lT%A1.W$,JY>#2:5qV]qSQ&'f^wLGfH12nWWkX"
"L(eA5R`m-.:S,lLLe`V$/FH?$*N^U#PeVuLo;+DuD05f%I?C/(rA3P8flBB#P$Rl8>OwP`GKPJ(=].-#.Ux5#XU9:#wbJW65f1$#L5dX$0rq;$>MQT%P_H>#]E/W$9/v,2vCch#xT<8p"
"*TRL2)vJ3%(35F%gfqie'47.$Q1)1#&>#%/Sk2?#:Rp/1w2xH#Z*5N^?\?cN'>L'g%YtJ'Jt%S;dvO4/iW4-(/F0n&O5IV+vjWA_MMu@(#;[I%#c(S7NT=IOCuk*H;%/5##NmQS%lu<9#"
"_Wt&#G=,)#<MsJ2'p'W7jd#^GljJZGC8M]GEG&(4_5e`*T_h7'1a8i)sE:K($fgF*MUR8%2-+$-#Y5@/tT]+%OmuC3QY;iamjV6p8Yfo@XBO1:P9Ev6Z3K%[c>vJ(8;2g:J?rI3hTLOf"
"LktFDZf,)TY[.T$VP:Z#XbrS%aEk;J?I4EH4C_C@SqQu7@tKWfCL$sAJghluhsXFip*-H-@$%:Z;=PE5N5>##w_@P%pr60#s&U'#/94d7/&Z;-A/dG2rr4$6;K*.3__3Y7`viT7^e@X7"
"k,.i;c[o^=[SaU@UaL`=mf]NB4dC&G5=;a+H)Ak()?-j'Xri>$Ko2M:.8O,4JYYZ,&AFA+Q=0/1%h7E+h8T%#ePrD546Rc*31#XAF245(M/%12?RWC5r)PT.$-AF*^v>X$eim<-GbVp$"
"lpq0(I]B.*G[Ph%?;Rv$8c[:/0FKT%t>u.uO@0f3>B&%fBWBNai5G);RCB'o*$QD*K@*,9_U+Ml]+YVQQ$kA(XIQAT;c4,RN%'JDM4712Mt;d0TIQhF/EaD#KXR@#^U$5E3WA-q)lppW"
"67rah(':BiO+,##Td]A$*ZK:#)LvU713Vc#7t=j'N[2Z#n&BT(W@aN=+x<2K$F&;mWH+4#Ype;%dg$]-M0573TA7q.iRRC3R6+=1;7gLp%[G)*uv1_AXt/A=U1`4'$Ph._tm(-&>bKfL"
"Cg`v%$+gx$6R$Z$8I_>$dCsV7Qvuh1*Jl>#5u-s$-bOm#<tiDBkvZ>6OPo^$Hm+1#mW'B#5_s-$93=hu$?CEM<'BVm$a[LgT[$##>DJvL]m-=N']QR_fe3onvB$XL9[$iL60=a*hfp9M"
"M7Xt(/5at(08at(DoO#$SaJv00r$s$&KU^3**387Ynds%(<4:.'.$$$Z:RBNJO>I$7Oq-MkuUH-poAr-W?#@KSA#5tdmK-mI5G>#@u%7M7g,u@`7JC#I#jA#H/AB#%G$fq6iwM.px$GV"
"[SUA5G@2022(>uuJ3*]PjvaB-h9^?.(AcY#tHU&#dta3M(OE>5DjH4XoKQ'#NBvLg?>lf_pcc'&gFNP&#2s80:XWP&2oXM$f+(hL=Tjc#SUEa*=%2<636TP&WCSP##M<5&&a/AmJ/$2A"
"c3]t@t_-9(*`sfL1^1N(XxF)4$w+F%dIVc`UAo=u$eFSnX4<gp9*:,)@_eNucC%%#'4]1v,TMo$YAS,6>*;5:x9b;?BPkGDjP2jKP77T%ul2D%1VP##VL-&$e54*$iaCv#^8OG2nh1;Q"
"30w4]Nu?s$x%pK3ofZ.2&MS.2,a9D500e(+w$+3Abq=T%&A@W$g%]Hu'*dN#x^8au'Xhn#A<s`#[rhRu;VTquiKk&#aPRV6`>viK<XOA4*9&##tj;YPmA$)*/C>/(f98$GV<eE.@]G##"
"]9#6/-G:;$9axnL`[F)#;[I%#cB0?Fo%U^#*N<T'O,wX.7OJM'Vb@f$6'_(#jrP3#wsU_$:r0H1q9L$5,KRJC?AY;$Ei/[52h:I3lH`228Yfo@QW'B#d@bbr2NnW$LxSkL4^H_#?9<VQ"
"TMulSv5#,29[-K37*cT%*'ED+hV3E*^f_V$2E?j'b87l'Qfn_##VK`4sMcA#bfU?-O'TM'&&(7p.$<F*Rw$-#TB:kFfY-3:a-P>%[=8C#OM>c4,08u6J]B.*:>`:%lYWI)F5lp8.mZp."
"ip'xoQ28l1NPu>FRnT+*.(o[oMsm31Rl$WGX6Q(+Tu07;_Cw-$j<I>#WKo+*]L6WGc36s0ed36pXWO(+Urb>Ff?2P2wTeAYA`_7;JZ].#40l6#bA2S$@HY##ax?ZGvtC/:l65X7-D8_4"
"-P:;$)&###*6&$&(_ln0LXv]%pQmF:HJ?\?Z3foG#0PlRn7G)L>3*(L>_fVL>JcfcMVpp._i2:L>13EtC/OgM>KG1W-^'$@%4N`c#AL]'dWV*&+7K>+#j:^l8%*p]=JCvu#G`%8@aG<0("
"=-QK+b>d>#,O$d)L/0Q(sOks6W@.W$S2RH)NNmA+FhkG)9mfJ-m8;S&_vUjKn<d&Oq:Vs$[Yvc(OO$Vug$&(#gx#K31O0CQnGUh$^hF%$Z*wU7eETZf-&hC,G8q]==PlH`WW['LbWC07"
"-/$$,>$b5Eb$:a*`i#$$$[]&$uQk*b]#vYY7-).PL,>>#tA.m9C?Rd2w<b[7>9_5J`UuC+Dl=s6TH(k',.OX$Jdu/(Hne%#$$SrQorSB+Ni5r'n:#j'V/$fZg58.ZQFhI386RL2+J1N("
"B-Tv-3/;v[Fn0A$3IxaO,9TL#V0b$Mq_]Yk70`K`T_am&<k.kU4x<nW<&ld'RE^9Vjiv6#Z6X&%J@%%#TKWV&bJ.L(:n1ZYYPw-).u?s$8Np5&Dq/m&h<.x,X_:kBn2;j0UjlN'Qe<5&"
"YT4p%s5Qm/TF=3pkP)tm2DFb$/.MkLunPGDsDkM(xsOs-,QxHD*:b'I7eWj0$2YYu*r]e$MHe(WQRaJM'9k*%a2ba$xl]=uh-KloC7vu#dDF.DRYLB#6>eC#s2(*M:Ep%#pAAtuL;TM#"
"m(NU/aR)##fGp,+R&I-27;G>#IM(v#[D:,2H5]t@Na_K1OT$fqpgf]+$S;,2I;p9;@<EJMB`d##?^:u*R>]%BTJl>#?w+j'RhDZ#f+Bs&m[9W9;A=#-eQ@dMQCta%l'OoRPH/9RwRVP&"
"MDW]+(g:D3X2t+;fhjAQDej5&e%tA+W3Qs6evY7&/7dC+x8/u6/&89&O<AE+@>cv6#0Kq'C;2s'*v&rIWXTsIpKmtI,xc$%A+C4(pinp0@*F)+B^3&+^Y*+=rd9E<*K0[#XpmT0C:XY5"
"sU4)#fU5l1e[[@#J=w_F61$H37:$096+c5p48t,#w$-cM$V/V6$j>8pC@Es@Vi@PJi^u_F:g6s.u;$Y-22&`Fo^Xv6Zl_#`Vr38]UZBT2i-M@jDiN@jiNirN9_TVN<`,:9/Cl9#3b,:9"
"N8lH;,%8ib+@l9#rY)60@EPn&Nj9B#c6X;;w>A%tG$D^#,_VN9]Df'/sox;%tw,`G>]q]Pqt_'/eW1n#gEQ*N&<n'QQu0*#N5XbGm]c)OPa.JhQDGq;xR$##tdv=P85>(/F&`i4Wls,#"
"d<mAOAC16vrl-:OjjqsLK-6`MZYp$.s-Um9gmK3()sY7(j3(;(i,*U-b(nX.auQ:vTZI*.7q:#R:JV+vjWA_MTxda$Ka>SIG$D^#5$>^GJJF&#P`*)/Vc^uPmj2p/:_AsQXt0*#wo9C-"
"?$>G-c(>G-dJ)Y$7fjRMm=-##m2HxoWg`+QA@l9#Je4?-&xnt$*mILGf>ou,o/e&#c#F$M0@xG$.6>##?^K[#Mn&'-OM3X%6k>A4t[)>lgUvr$p`;W/3q*&%S&h'%ru2)%/M1v#3Nka$"
"KsqV$7a]>#,$ZlAt$T'AMCEp.8Z4N3?OwFH6EbCVl]>KmtN.@#$/G>#eZvc%;<op$woVw$:f[%#NgsA5]VgK:'mg<$l2Qr%_uUw,[Pa**lC1-MAKHo/W8:'#+E&7pI<%gLAnN;pxMdA="
".vHa38?;HMNfq;0DTed3<9Pb%S(&u@<?\?_-Cn8s$&Gsa/N,=w?tCsa/K,bW@k),##TALN$SaP`/@8>3^%'/##KW1T%FXs&3UxF)4>C^*u,.pe$#eC>#JN#N'g?;#P_?]f1C%@_%hR&a%"
"a+T2'w_]20<_j5&^MfN0w3]oq9v<<#L4ji-sHI7_$dh-%`-*Z#Fl#PYOAQ('YnZMEmrN1YSL,YMCV.1#G]bV-8v@cr])'v#>Num#(:_c)&5NFM1SZ)4#ocHh.Tc_bxIAQ/`pFp%tN;p["
"Ej?D*_Qb&-2LHx%[4j%&M2[d)W=A30SD-(#W:ZlfSDj&#>ncGMp@h4Nt7lt8^$`s-RlTJQ:1QGRNeVG?jaC,?UQ[5A%7%PQwwU4QjgC,?\?>+sLKu0B#5XR)$Yrn%#F?8W7aDj]+4Y<>5"
"A00:%6Ti($iJL'#6t:I3la#<7],%12vk,Z$)aD''1U^:/K2QA#n4SNHvPZZ$p6W8%<i?fuP<ZD4Q<IU7vLi`>e]C8Id:$v#m7'58df%297T]._#E[a'F$qfLO@i,+0b;<%/64*$o=n0#"
"@,i+Vi7K^#Z.ofL(fH12aeNt@d]aI3j+,i2UY<n0f]_P%oL_fu^S5f[+Fmv$:o/M#Nd/,MmQwjuWo60#da7F1W^K>$<cSA$YoNI)[Ibn-M:fJj&I(DNvBre$C;gF4Gqj?#@]WF3>+ST%"
"MDim3#8cLY-iI-%P--^.7,mYuU,]gU1NrG+@tBD=nFY,M=At[$Xkth8iLh'>9p./C[68;HR.2s$dbT`N%:pd#$OAQC)IccMQ`=[&$q^#$v@)v#Y?%SmcB1$MUFPL#J0G>##)P:v:(<rm"
"3c1?#<Y7A4Qv5H*P(Zc;.XNs-J:tC=QsREF90w+iCjLA4w0`'/<G-]kO%Aa$t16f*I*;'#JU$guwQ2w$DS%N0]F0l'1ZXZ#X(SfLZGaV$QS=v,$9o>#FV)E6Gk,87jT4lVS.fo@sv^?f"
"xHevZddJmA=,xY,LvrY,%/5##6PUV$bw<9#ZWt&#wi>(5SrphCM=.=$Kxx;6ZY&B+0s4v,2?LsH_0@A,R08%#JU'R/h=_Z/iEI],Re39%hS1*scvuC3bN3*s6^)22XU<8pe`GW-cM40="
"A*vM(k;oO(<MfD*1Z?a3Jc7C#,c?X-''3oAeUSM`v'xgt=G-1c8Cp4jmEmuS<i^&J4adU`$_<lS=v6oAGDh^#M485Ag5ij*P>Vm/Q[vLu?)co9f$Lj%wZ#YlhV8Z,t;Yc2Clp>@*cO6'"
"U,d@-@-.P1ng8:'RsWq0pO?s0owIc$Kfqc-lwD207<@h($2'^+OOrv#YQw:7KS$]#Svfw6^.FM($?ux#UJ`k'$xGv,W<6R*Tr'+*5umo%C-Y,2/6H126e<E28ejl('?%12#Drs@SfSh("
"H7i0MBkU%%%;18.K4?3<(]7C#Xe_a4`fC*4%TID*9Ze)*liq[G:,Is]J:_F$k^;cqJdMW#^6&uT#r0.?KWa8pgg<H#J+:$/7c[ET.rF%3R4.,;]CWMYpg>WJ_>,c`/QYM'ZucS%*M5rh"
"U[_dPLp/4(pfW1L/4biu-#Z.6fG$a4I3F<[KB]A1#)>>#-G:;$cd<D<TZUAP/tb&Q`#C6&&V.6/sM'F$/N.S<RULAP-$of$QB0E<cJF&#8r0<&aNs$#6<6^7XuVM:)g[0Lu9JX$$U88%"
"$VAc3]esl&It/6&o#6`erbbe)7l'<&]$f9.>['m&Y[.%#vre2('#vC3eN3*sdw(?8hM;s@`/rU@$>gG3VMKc$j#HT%S)TF4H_E;$5$=/<$C?ocU9/Ob<Hd>f@uu3LbClII@/g60?+aGY"
"D^`aucq<puOAW*4TolKE&[A@uq#p60I*MP-p(Wm.#Z.%#$pZEP0G^Y,JS8M5j-^S)tfZ<8pqZEPxPoS[5$rD>R_V1:ne>^=HNb&#T&LF$3h1$##_&.i^GTZ#Pu[+idYpZ#2$`V$@eGj#"
"piZ.26PYh2#VuGGm4p%/[OQP/nnbu%AF`V$GvCc;4^3J#=o2eF9PTl]^#]Ni4rkA#2i-W%X,>>#*,IA4>,OJh]wDM0#U'a<>C7w#57JP#=c1Z#[mcN',:=,2Zb-/399I12=GvC3>.YT^"
"6IGH3j;W.3%IuD#'G+F3P;lLTE7@eE@c[$$l`OA4Hr^(G#6c01Q4s'Mx-%PfJ.)?55,N<#mC@Z7jXK(4Kg`6Al_uKC8YS4KugKr%I#?B=V2/#,/&M$#IweS%48b**Bh:I3M-W6pn5UbI"
"?#YLMRhYV-_IPVC7Lx;6Ynn8%Z%:W-IgXo%;G;1M7V%a?T1'k9E[n>`vE@oT4R'^#MTdGFpki->g<G&-^(iMUKcl,@3OF'#N_Se%A^:G`GCef1WSu>73uf'-DVhu&p2FY/t)kB.@5QD."
"Dj<h)?KQF<]7@x#CTfA,b.Ip.GQPN'O&pB+XT4u-A6[,*/h=p%[Wt,2YC:F*LV:V_fGja'Y`Z-(0U^:/c3rI3luHb%=;Rv$XmA3kl]B.*-BGsZZI)v#>B2D5k=^/Xi)Sab6Km*Ij#H0G"
"R&_n;lF,h:G'98b#ft5Mj$(FGRd+^,R/b#6G&7D6*%c^6*XX8]T+?7EgbBV%Ns&##'+85#(=xh$VUl##YvK'#ZIf*5$Mdd;v-[iCh?a^F*@VpAYi.>H@srp&qCaC=x$5*=KhjP&)Qb;$"
"<[SM'vt0n'M&#B,sWgi'E-kp%kXWP&W_DM0(2G>#9Z4N3Q0MkLEc+p7HrJ<7MF<E2mmuC3U4,L%1>>K3/F']$<e_p7a+Ca<ix9CgEY_&O$.cbr/3rZ#><[@@)m'`7Hi+'Q4/+WS`58S="
".RVl4N?5&4qrT;1Ri[F2$)dF25cr$/;+-V9(COS]$>fEJIJhs0bJ$##w+KV-k7[i0Tke%#EVM)5Z5w,M&G`7)#.`,)^rV(=ql%1(vTVRs55xL(5J^B+fMR-)g/uH3&*AJ3r&BD5n[I/N"
",<9O0^KLW5E?RL2e4gxOxvHg)^nS=HC$8cnCw9FS%&+Mg';qRe)abRj3#Haj,Up]+eY+;Q:/s^fUIv1#S?Nd$?;q'#=>N)#OoZf7C+54:FmUeEw&4jC[`ieEcJ3NKa'$=Ioq:8'F]qu&"
"m,=Y/&f#g3,O+>7WcGG4^7941LK_=8v68)=t4Ad2h^mE3ST;S0bC%E5K)bi2V^*^m`9oo%eM]t@%Nps-7rLkL>R,@5T[(tqb@,87a0*:8weW/26dFT'Yr8H#u=c^$'u;?#bYxo7<xv5/"
"V5aM2vYXI=*IYq.%G)JFBDk@K.EP(?5lGN(7%vtSxCeh<KnlI8qHJ88uPdT%$cjo9P31?-0*lJ.CL2e=#=5=8^Wh>.G6;&PikCM+UL:f#6a`[$]Lq='Q,GuuZ,^e$@&p=#OcJb*S>w01"
"5;kA+f+6W$D:AbIR+IG2q3n0#=Wk-$deON'2P:v#;Fw8%<oU;$8(`v#^jJe$&b<1#>?a<%B[*9%@F[s$2W=_/d_NT%CX39%gU=gLZHx8%@?f>-+lLP/5uUv#8C3p%5bQ?$94Rs$;17W$"
"RUt?0ePFs%Z>+KM#.gfL93X30=Fw8%;=[s$He^-6bqsp%aHn0#`Cn8%<7@W$f[FgLRHRuLJ.PG-P0RE./MlY#<XC#$gpWq)O]L;$5mVE-P;C[-ITR$9__ET%2;J^.<Ces$$,B;-DZtr."
"EElj'hU6w^*YX,Mu=o^.8rh;$>VVQMR'/W$hVxv$CU<T%WOp9rBl2eH4;<X(mpXtCHk,F%'ID<%7(R8%BUk05-YMtCgWJ9rOTRv$g/g*%KxGb@h7T#$:(g(NM#:6&8M4_.HnWT%SP6X."
"?R*9%QNE'/>+r;$mc_$'G^a<%H0B6&>7.W$<SgHQ'>ofLuESs$6oqV$DrO(.e[tcMNxNq-'R-L,0D'?$$C:HMK^49%h:ffL%vSvPT/lp%DeWT%D_39%?.f34uBV>nQK[e-B,S3Op%h;S"
"m;3U-oPj6.`:ofLmG5gL0>f8%cF*^-XUp9r)M4gL),eEM4PP8.KqE9%R%hs%Iqjp%*ms63_UNp%?+r;$xL)F7n80X:etE9%(WH*M#93U-G__).(:Qq7VYIg<]Efs$sn`e.?CRs$2;]B."
"Qvcn&jF,Z$U=d5'n23U-o83U-*>TV-0qKwK0f=gLRp+9%?4.W$Eqsp%meOgLJaOT%,^H*MhjFT%=:.<$eC+,MmN>gLP9f8%OpsS&GI-W%QS6X.Gw/6&GtlhL#@,gL'UY,M/bYHMndWr$"
"lB,n&Hhap%24?68]=a9DaOw8%G_*9%84p,ML0YT%OZP3'CLRs$<h/6&i_CW%cO=gLr(8s$w*kB-8')m$bqjp%>M)QMHWFT%hHA[Bm2eH)4gK[0m7B9r(G+gL'6AW$M8kn38wMs%nXu^]"
"LLn0,[O*9%r83U-$#9;-.PP8.@h/Q&3.B3kj%[$9`b<T%*X.T&_k5:r]h<;r1xkgLCcdEMI?69/HnNT%l$:HMKjbT%7uj-$jNPN'u3LHMOvbT%?Ces$=CvW%EZ$H)EeNT%+&k6.jbkGM"
"w#CQ&gXO,Mj3$N'K%VV$1?lK.I[@<$b_HT&:7Is$(O#i#`AnHHi996&Jnd]/C@Is$J-B6&U639&J*96&.;^;-B?f>-/9BoLY[Fp%<=*T%*wFg.@Fes$%.sI.F$KQ&eVXs%r*1HMAN+9%"
"25P&OiDnA-.oL5/VHBq%s0ugL^2_Q&@or63GfGT&GOes$6'&T&M<DiLCt)O'FRRs$D7`BS]XEp%2#,t%;+@s$#fFgL?:#gLZ,_Q&9PF#$4YY8&pBnBAb*KQ&Zu:[Br]sE*(2(L0Pj10("
"@Cw8%ZCB9rrj1nW.O;<%NQ,n&()#REnwjp%FLe8%MBpQ&rtbgLrs:HMlH5gLV@,-MrU/R.G-Kq%EKO9rj39q%Fe<9%UP6X.NB#n&ZTRI-*Jp;-tx8;-AM;6MfgXT%hmj-$rvCk'R?pm&"
",2^V-,<(kb[D'9&MWcN'A2bw'3xXgLA+@20[jlj'9u_;$UY.$p^Uw8%0MBZ$#<jbNwg2#Mrp$&OpfXT%M'7E.G_ET%:.?[$[w`$#<Cn8%WjYn&vKw5'nN;#PUVQ3'w61hLN6L6&K$O9%"
"k.$6'SQ53'V9ET&U?^Q&s?e5'7Nh)0Gw86&<CRW$;@rT.CU39%tA3U-Pptv-t-lgLODfjLw*/W$ii0K-<1^gLm1j0(TH53'-2^V-&J<X(rNpQ&L?,3'KXns$xL)F7vO$6'_v10(_ahBS"
"-V+gL>VdT/L2Ea*M3B6&0Rd;%)US/.)-s#Pc'Um&ohFgL[hkp%5;]B.f74M(%_@=%V2rg(DUET%Je3T%pKQaNYdFT%VHBq%%kL90RK53'L-^m&?Eq<LjEpm&r50I$DwDe-2uXgL`<C6&"
"D_eW$INuJ(=WxT&Ew8Q&O996&b:%O=e]'T&.dH*M9=3U-,s2bM%LLQ&M3KQ&bV7L(X2rK(7xQ-Hu?^Q&TNgQ&S#V0(K<5N'vl_$'6NOq&_#?R&ICIW$NQ^6&KwA']jh*t$VDVo&PB^Q&"
"Z*%W%=`04bh30q%&vf6Ej?Bq%qv6[B$AP_+&T/R.^,24'x^Wm'qWD#P^+Ek'Zpcn&x6ugLBsugLQMmj'9QOQ'TT>3',m&UD;L7m'/V>s%BBO9riZ:g(&#9;-m23U->_i=-n23U-kXa>."
"VjY3'Y'Is%I`FP88HWX(f6B6&3ij-$@_ChLM5Ah(9%R-HY<=8'T<B6&Xp>R&RpCk'Va5n&VZ,n&b,HR&/WH*Mi^F9%hwZT&EF@W$n?$:rtvuN'OqwW$'CwP'9?4m'0M?[B;]:@,D7w8%"
"OaPn&ShEp%O<t9%H0gm&P(39'39MhLXjHn&SB'U%Ts:0(^sJ$0WgY3']^G3'E?f>-LwDY0KtE9%HUn8%uB_-ME*(6&Qp:0(OBpm&&Y_5'U2(@-`3uSM5W+t$<.*xBh?K6&6rj-$2&+.)"
"_Se-)fMi0(Kgl-2Zd5n&YDIh(PQl/(^&2O'Sc:7'gu3.)T9kp%M#i0(M3Tm&9VLq&DREp%g^G3'qe=gLdlDk'wDNU-6J+?/RTcj'twXgLZ^dEMom.H)w[B9rjW(0(-X(hLt)MHMpH;:N"
"jANq-`5o?9va5n&S'bp%]2Vk']g]Q'aYnH)$=:-MSPQ3';Zqd/Iw<t$Om_,)76Sp&l_=gLjRdX0VsL0(ZmY3',b$eMWcl,Mk5Eq-uCnw':+vS&9WFU&euNI)b)M0(k]71(,S<X(%02O'"
"Vm:0(U'bT%;TX2(KQ'8/elEe)3wQeMV8&3/O?,3'_FB9r,HZ3'#1lgL8#c?-xDNU-C;rL.T^(0(Etn2(s'u,MpvId)%fK9r#tu3'Y$k5&SNBU%tNd&O<9:6&bv>R&d);k'SCl@2TTlj'"
"cS.L(_/)O'cc3e)8%Z8&#?[t1KOfBJtRT9r>JI['itaT%YpYn&u*lgLXYa>.aAVk'SJ[$0DJ_Tr>4V5'(qW6'pVD4'giRL([OfrLGw(hL=?_Q&agpQ&3%IhG.3;O'nC'f)YN96&Q57L("
"QE#3'*IJJ.[=:<-^)g-6+FRT&WmP3'8^#pL):Q91#IQ,*<hap%f10b*v;Nq-f[K9r/-O1#pvhg(ji<**wnDN(aJ7L(o23U-+#Gk/`5Mk'f,;k'0mlk(ZplN'Q9tT%Me3T%#L_hLn;Cq%"
"R^Y3'EiOC.[87h(j.6Q'8gxZPu/VHMO*872YvuN'biN**k5vN'r:k**WvXR'bPIh(YW,n&ds5n&_DDO'53oP'$u;N(v+O**)<#k(X<'U%skOgL;WJn.q,;k'7%vW-X=B9rwDi0(CDI./"
"UBB6&0,>nE&'dn&h=B+*gM3E**lK9ru>ik'oRB9r?eh-MpH;:N`,:6&jD24'%4T?.dA.h(xgs2(-eU-M>$3O'loR1(cOB9r>O(hLxNdN'[gfm'EqEt$K<gK1P00q%b>MO'VQg6&Z?\?e6"
"$ZRh(UdXkk0`+gLCjXZ-+ds9D,0Ke$#$)O'wu.l'n.+i(^OfrL2oo-)]jP3'h/Z3'bVik':[Lk+,WIh($>tE*EIRs$=lqKGuRK9ribes$quMk'',%@'rvhg(jY7L(1KLObmKK6&SX1H-"
")sFk/x%8l'+$Wd*1m1L)ddbt$]SwH)K3KQ&wRLE#u[]1(/bLhL2%.7&aYeH)j]R-)fP@h(6)'UDG*Ee-x))O']#)O'^XT9rB[(hL[863''<>K)[#2k'-7*3(fS@h(U_Cd-A3LwK5%sXC"
"tRT9r/f5c*qqIK)$MtI)4`<X(&jRL('<>K)`E9q%C$`$')Z71(#(Hc*mnK9r2WVO'UQ,R&[2Dk'cqvp.*'g/)nt>6s(Lw5'(O(hL'H#I)cr&b*lmKq%-Bp%#NE#n&&fKJ)S)`K(Y9K6&"
"t5O1#e;-n&ur.l'wx*.)ouEe)b8@-)sIke)nxeh(vkP_+g5mn&[7@O(d8;O'`)MK(TT(0()M%@'#tcN'f(b**cArK(1Usj(nx3I)6khHM*$;-M'F5gL.?rHM?MJF-<c&gL/3N`+v.k**"
"FH@iLYhl-2rrIL(rkYC+dY3**u:bI)X:nk(-[``+h)-n&^.B+*`,MK(>.*O(Q-Tm&,dRh(>LQQ8gg,K)0X-aN-ISh(d`[L(Od0O(g2Dk'0]=M(>5uB,+907',($g)W;.h(_KpQ&v8O1#"
"&s&b*+(v?,8^q.*sLB+*bS71($5oL(u%oh((oKJ).FlgLN3DhL5NAh(cP[h(PN^q%)kn0#sZ,R&p+xL(-UChLWPE#.:3rhLs[eI-/>^;-7%G`N#H]h(x;'.$VmRiLb$j@,MFR-HwXx-)"
"x[9f)n7Bb*ms5R&(]Tb*wR^b*,X@x,&)85'XDNa*is,7&=K$E0Z9bT%-ooh(.5%@''jaE*&Pte)(n:(Aw/d3'GV#.7n]i0(50AA,<l&g2Yt]Q&nL0J)u&dj'miVo&%:(hLXdiS%'ApB+"
"3O%=-)x>(+%u>c*hx*i(-Yb.)'JbI)1CH,*4X(hLE1PI)p%4I)jrNI)S^pq%QwjN0d)v3'xOk.)3hUhLYc&Z.*($G*12TV-umn0#nnLs%N[YHM>.;hL+(.m/AI_J)4e;)+oA624xUKF*"
"$&SL(+.C/)v')D+(f0f)[>W6NPO_J)kQKq%tek.)/XUhLxmOI)#T6e2gG(K)rZn0#;=Z(+qtCs%S_j,*4*.iLxDGI)qV7L(BOhJ)N4n[-HZG3'19bU.(<Nq-vwK9r6BO1#-J5_+7bIX-"
".*L*+)1d(+i(4i(]ah<L8fgb*1rkI)5fvE2'AXI)s4XI)c2-r%lb1<-=v,g)48rh)%MOI)n%=e).ogF*.:Hc*82eQjE$r-M&Z@IM@MJF-=O<j0*[[X-9lte)DK=Y-W*@X(G46G*xeP_+"
"wA)4'4=m_+TQ'8/9Bt:.Rv</M*m@iLp>xd)AvX$0FQx,3&V0f)*m=k/)O)D+uMHr%QXWg)uFpB+eCAj)ui[h(Fb6g)]3gu.Lv1k'=#?o/kA7L(xfiK(M=Jn3*0CW-,qe],>wr)+5FHG*"
",`+2(-_.x,:[V@,3C)`+<O$G*F<Ee--X6c*%VBF*uOgF*Z)Q7&6i^F*mMik'/76,*u+oL(5LZ(+8n7A,.-+1#4KTe$%Rn0#BYR['2$o0#H'VhL=Mhb*ScHW.@g^V.[_R-HVhXb+2ctI)"
"9nd,*-ke],Ld@iLTH[(+ONNd*um#7&*O6,*quwL(%x,c*.M]$5jc$H*J:>j`,$N`+?WLOb/&JL(FJ>nEQa[iLKUes.B6gN0G*hi'#Rnt-@ujS&13bu-hc[-)cQuh#sO+.)K'wG*O0AA,"
"B0+Y-0(;@,HskY-B<AA,NVMP05x0/)f<Pb+2(h+**`pB+r(K'+=%&@':Pt**5?Ou-1=v_+cTAe6MZkY-56+1#7TTe$'Xn0#FfR['FBgG*B]rH*W@oQ0OKbu-qrtjLYhl-2H3j`+FJDp/"
"1X[t-LZO#-sMwD,c_o609l+.)(EKv-+l5_+(:)D+l>7-)d$/A,W-[q74vW#-LKw&O1)=B,2eiD+4FvC+5ogF*h>,b+*J0n1UVIh(X_',3%/te)7fOe)q>2nW*0CW-DJ29/ZGh$-N^xA,"
"A[Lj(D,6S/T)LV.K^ku-VjA&,QHEe-R'&&,;e``+$Aso&2TYK1OObB,)42@,E-N`+-r%O=S^O>-SA_v-9)9;-$ERuLa-Q?,Les9D9WKe$Tjsj1daAA,u'Pj1G^$6/GWXY-Kn))+UGY',"
"EA29/Pm+^,Pj4#-nx(C,2N)o&Ap]a+EHRiL9YAA,F?SO9F<SA,2m=k/LY?W.@oJp&IUBQ/>0bU.=ni%,CeZ(+rdUk1%4TL;$3[h(B,U?-LYbE*@t-k(.[[=-**CW-N(J60g+3x-ZDC?-"
"M<eg)N]DP0`YZS/W>-s.cGP#-UNEe-ZTx],D<o],.u4m'=>7e2QbB$-1b@=-Qdf],2%&O='-)?-^2P^,7&`D+0'4m';]W>-NwSqD;^Ke$SN=#-.YI@-q-gU/#PfrL[Rs50GqM`+a^j`+"
"TcU?-d?Mk+#d)T/1=C,3l,Q3'^m+#-.4hF*>:1j(agu$.PmX>-6Y15/i(ws.ZgwaN5PK?-H^pR/HEf],L6N`+*W<.330rF=*B*.)JVQw-W+U'+IH*L)6*Ou-**CW-X[b31seSu.fr?w-"
"WjaH*X=oi1nF8m0co;p/oxLZ-ZTEe-X:V0Gegk>-44Gm'EiN'4U2Lv-8*+u-Z5Y>-6+&O=(9Vv-$J_gMj$f.Ml>a6.NG=/M1MJF-HO<j0e9dD4(pLv-<^jE4G^$6/]f)T/`joA,mb*]-"
"XOb31g%@<.h+es.5FOx-E8Jl'U(r?-Rp*jLAILv-]xiC=hVFjLEPhF*g->n0Uk(n'H[^m/Jtv(+Zpb>-ZDuB,uSxF4ueY[5u2EG)C`n.2lmV3(VOt/2,1QC+.]VS%Rce.21*s]5u*cN1"
"n$l31H/L?-&M*=.q(eW.,_lr/b&8iLh1es.W5_V.N2Zs.x_Fm'kwjN0@IQG*g.nw-aJFjLBJ&q/v6:k1JT+1#?mTe$5i=s.NwSqD?jKe$cSHW.K$qu.4Tel1+PfrLhWL02]vX>-xiu>-"
"iqjt.pQMk+6.uj1OgJ^5$gi0(so-W.<q)D+P-[g)$G0v/fuds.9Y15/-@>R0evwaN:()t.[].M1ZDh;.`/5#-F=``5]<^mCA85c*jQUo0x3N[,guPf*MPrl0**CW-#gFc4Fsjm14huR0"
"#JVC,x2j&5=?3*40b$h2@t#70idEe-3iAq/nI8q/LB7k(aT[q7vnoQ0RP2P0(%'q/FF&O=129Q09U#r/@xS;.3B0j(FhqP0E&9;-F,TV-e7;kFthxQ0lP]p0S=)-44PfrLwufb4s(es."
">+4t.,EVo0)hMk+FNj*4rLwT80;]h(8.B60Mjf],f)9e*>3Sq1t64eHN0AA,Ij<i2vG=bN@_wm0rngc3rXA60x=@W.g5QW8$8BbGMV(D+&WjM27wxt-#o.d+^ItJ2**CW-7xv]6^1;L3"
"Is3225=o%-8MU<7TPY$6EpJb4W,/l1rpEe-BnLO1+LCO1[5bh)sfPL:4t$12cL4/2:tc31OU&O=6`l/2Pd7P1DFlS/6^,g)K?N/2J&9;-K,TV-oF;kF,k-123r-O2lZb'6<PfrL-%@]6"
"0uJ60U0$70ASkM25$Nk+Y]:%69w(1;:cOI)M0Dk1^`Lv-xrcb+T]HP39-%12@Y15/b+mc4/mXbNE6TK2/t.^5.O(O17FK60.rt3;@3'VK[.@A,9f1-4JaL7/6nka,nBv)4**CW-MBus8"
"wN'G5a+Hg3I9L#.Mb8R9lb*u7]+r[6q@CJ3&'Fe-CsW.3=E3i2m.?f*0%F(=gu(e3sKH)4P)oh2Ye&O=;7Id3hrK/3Iq6m0L#9;-3#9;-p[YHM],:kLa>a*4NEa.=$DGH=X?T-HFLh)3"
"j46,4gfJK<=/6p8s'FB6ceIE5H3A/=M[49/-hts80R-)4-Q/f3^ZK7/HdcC50&,jLwj]v7rtw]68>4i%:Kio0E;bI3T[8r8ST;b=sn8`,iKn,#].8Q0O1uZ8vVx^50Z$[-f]P1#KUoQ0"
"7:w03>L0Z7w?>o9O/qW72;A6:xdt>7:Le>@Zs.T0=Tq`5S)FM2B9WA5wkYD4]3*)5BnLK2Xj_1;OJY_5_A;<7x^589k9xLMLFq0Mk>a6.1qe1M1MJF-^O<j0%lHY@NIa`5o)([@G^$6/"
"l<TV8r`0/31j:,5fPs3;'[x&6(hk>7cx.d5B-K*+_^8I4:([lLug_`5j_.'Pl[7a5]To0#x:]6:Z[jg*H[^m/L*QR0hc>G4hXv03@&2^@?#H2CC:48.C?)F=4mHe+fixG=qq4N1I]VS%"
"FKvE=dEg4C@S'k;5M94;MF7*5NWUC6;n'$7YbU<9VF;kL_jSW7h%6g3T2Pv60XI#6rA5^6Td3e3m+KG=dOe=7&8Ju88p[2;'kOMMP_?1M;j@iLl*3^6[uM;7].sV7w$Wu0=9P5Ckg:Z7"
"7`A7CW*@X(5rFr9$Q&G=u3?V/Z.$H=I4%;7g@TLEX6Bq1/S&D=i<-Z.RJq<77^Ka+nnE?7f//T0q)8H<T`+R:'')4b+*Ps8fTg_6nw<#7nJ#G4;F-_7H),##$),##D^dfVige<6<gGV%"
"u@58.eYC901^vvZCe,(vwK,Z&gF]'/PYu##UE31#U&###2'9]=[N=01Wc(MTDxNfLZdR+ND.Dn%7FL^#K7YY#@p]&$qj^#$)Qk.%#@El$2i*e%Z*Mg$jBqf#FrI1$SVe#M]=0vL/b;xL"
";@(q##5/vLobgvLOLv:$+j](MBM6f$(ChhLo<ZwLqa;xLlA^nL2JCwMKM-MTGJ>8J>ZnAG$Anr?9Wfum^TH]u.4>irn<xl&%9(GV1[-S[f#@A+nUj:d2NbuG2G-F%#cf:d4Xk:dDX]w'"
"u=f:ddla%OKH+/1,[Goe5c8R*RY,F.um4F%]o_uG2XNF%]/juL.wi/#GxD<#Y[B:#-+Xr0q#HS@=Sbl8px#s$KoUPK5-^lpehbP9uNtxY^r4j_;PT=u7[R'>_>8GD3Q+J_gZMcD?EMdE"
"V2o(EZDMk4jCQ`EG2#J_]>j7[TH0]bP-X%b;fK/`W_)J_:b$?$COERM%nbZ-Te5F758hP/T8Nk+M%[w0glJwB#x.F%ZYl-$pu%a3DetG)wN')W(90F%G^Rk+&0sQss;cQsj.B9rLuKwg"
"LLsQa*?18f%h:/$/kw%cU=b.$SbrJVCa1lojAQfr+4$Z$kXc9VJd4R*/Mv.$uVY+`nB/MT3Y`+V<7Rs-_OCpL?)B%#RR8=#:=###uG*j0PD<YY^xJ>,DDir?&=KgroFRM'>o_uG-=efV"
"WVw.i_?/W-Ka7_80gFs-l[2F%^g72gYNdl.c/&5#F)m<-R1QD-a6T;-m9Lw-hYuwLNaC:#6M.U.jqBB#Q@;=-J[lS.pf[wuTO#<-NXjY.-$:'#p6T;-.*m<-EjF<%&t_#Q(m=)jdcCk="
"4*HwKGn6;Zln9;Z7LD5/9VG5/@+;:;g=IG`QJ`lofOr:mU9?l.ST;4#4nL5/@4###F,]fLs(#S.c)f=uI=o+sY$>,sFAPoR@[Im'&IjF%,4_#$8L)uL&MDpLGjqsLS`E:MTaP8.bXl4]"
",+h^#ldQk$HY5lLK_d)M<(i%Mk/YX$43wiLO+:kL*fipLuxYoLi0HwLPIhkL=WUI$k+_+$G>s)%jFe<$LF*7M3%3B1tUWP$5dZ0%-u#?$%g1p.MbsY>bHp^o#m>.$DZV)*`'a9`Z####"
"0:jl&d<cY#EG###[?*1#1eO2vhBg;-.UGs-+QsJM[+kuL/w(hL;]r'#>qb2v9r0hLVLI&Mw@%$vXCg;-jKXV.jl)%vk$)t-G>nqL-7F%vtIr]5`lH#6h9BX(KJ6VH^rmY6;0-p8L=%Z?"
":^CX(-_X_&Lq3@05EQX(*1ae$j7SX(.t2L,n3gi'^<KJ(>fj1B)B)s[3TOe$*2de$Y+Y3Oi+_-Qkpe4]IJsV.eM?8fSxS3OmHSe$vl'F.M5nx4)()t-N^K%MjA0,.[%mlLNel&##OD&."
"$6LPM<5voL*el&#i2H-.(NqPM/ZxYM*a+ZM+g4ZM,m=ZMvsFZM2go#MKPFjLsq3<#WZlO-*Ag;-&[,%.W]>lL%ku&#*wcW-pa=R*(&Qe$.LW'Svpi7[1855/v>dD-FIA/.FSWjLcDg;-"
"*n,D-6]-2Ma$[lLRv1'#AQ0K-?i)M-+Nx>-l:)=-G@Qp/ZxUZuwrB'#3Gg;-Pf=(.D-<JMsiY&#3qB'#gl4wL,+o(#>Ag;-mlG<-Ejq@-wYGs-t0/vL*I2XM+O;XM/6AYMiB8#MR%1kL"
"u4.;#FJB<MTp>2vc4pKMOCi'#w[lM1lNp+sh/@>Q3?jM1+.Dj1XrGq;Psu(3<gAj1nL]9Mm9w.:bMcf1Te`e$r,Ck=$)EV?h9BX(3?QX(/eX_&3J.:2Bx.:2+LLR*3@TX(.c[_&xEXk="
"(&Qe$-b45&0k&,26&)pAFLP8/pYX/2,A_e$,4q.C]MsEIXMR]4]HF_&0@+:2-'J9i]%0;6KD:@-2;)=-)Hg;-p>L1M@N%(#b'I/20&7F.Nx6@0,8de$,2Qe$==,wpZ=&)3t:]e$hDhQa"
"^%ku5='J,3[7`oopRnY6?+D,3UR6^lfDhi'):Re$3K^e$iiu?0b10;6v_<X(mqW_&kF`e$ue`e$.+M9ioe&GVubdfV#UtxYXiRV[Z2#d3ng&d3ng&d3V'A)4TGK2U^mI9i:]Qe$t<sKG"
"l*`l84'K_&PqI)4r,d%.56LPMdc/)v`/;.M(s[(#X9])4W0(GM+b52vfI48],Qio]%KT)4b1Qe$hI$REi*>A4DVWe$+BW'8_Y7A4L>DX(,0>ul%#-v?5YdV@ds?>Q)htxYW;u`4;5ODk"
"ssE'S5LNe$-JoFMn;Gxu_fG<-@Gg;-9pon.)U0#v`5)=-@%a.._vSKMRIhkL0k?lLp2N*Mjh?lL%j?lLF)2hLA3h&vDAg;-mYGs-t)KkL%slOM(/2PM>?N(v#5)=-+mG<-&Hg;-'Hg;-"
"+ZGs-YqboLI4fQM5H_.vOgG<-k?Ab-KdJe?[Sgw9kJMqV9wLR*4CTX($4(GM2PJ#M-1x(#Sx33.F4qHMro1#vRxv(#?QK&5[N-T-@R-T-Ah)Q.+(*)#Cm,B5Fa@o[Dlw],HF,W.v?J_&"
"&xwIUi*C2:u/SD=%gK>?=xvV@'H;R*@#7'ouw&GV+1efVkWT-Q/^/S[/Hmr[AcmA5^%Qe$nb6REnb6RE:FM-QNGNY5o;(^54i=(.[QsJMV`xb-LW+REWtg+MBD4)#k^K%Md7u(.<<:pL"
"F>4)#xfY-ME;4)#sGu]5BYWq;=FwKGWtg+Mt7<MMs+elLdbg5#]KNjLwE=)#LGg;-TGg;-vS]F-g/&F-R)A>-jnc[.b9`(v_hG<-pQ&7.G)eQM'DTVMnH2XM=O;XMENfYM4ToYM/ZxYM"
"LNJ#M_BNMM/3nlL)SD/#p.iZuVUGs-PfPlL@8=JMF]tJMEf[$vp?DX-d$8F.kvV%OqHU2:)6Oe$mL`e$%ZpW_(al%=u/SD=,^o`='T4&>(^OA>+s'^>hCr92%x`e$2<QX(]nOe?^qOe?"
"mHPe?C%/:2L;s9M9wLR*4CTX(A^EL,(&Qe$U(0kXIFPV6>W.L,WWFM0woEwTkXKV68a1Z6/t,wpvgw.:>s1Z6Te`e$#..ciSkwV@kOQe$[m,:2xo-Z6r@6@07d=F.r]-Z6db[_&;'MR*"
"(&Qe$@lEV?Xcfl8u<_Cs'5dV@tQ:69kO;ul$8IJ(VDn59&q;xt'htxYm*cP9Lm0poK=S]4O/wP9kETX((&Qe$uQ>X(Zl>J:Oe:3tCZ55&0nIp&qP?X(/et(3/'4m'BlfM:%5.8f9W'N("
"3TOe$+n*AF`.N;7+=kV7)5dV@9UuM:ipX_&A,>F.^gmM:kX)..Pb<ZMogo#Ml5KnL5r3<#l=aM-Y2H-.6?fnLgu,wL<<$'v$BSD=oOdfV(-QV[*<vr[9-A>-(Cg;-S8RA-S8RA-S8RA-"
"*;)=-*;)=-*;)=-lj&V-lj&V-9LXV.hjc+#i%kB-CRr.M'$axL&#vOM>#vOM<g>oLYxg=#g`=R*XVii'Vq3>>BBboo19kM(o$>p883K_&jV0ciwTtxY5Zmr[]UQA>$W6F.nRYY,>DJP^"
"x]0#?+4PJiJHJq;mHSe$QIs92@x(F.9,&p]v_<X(,2Qe$AvlEI@.FV?J1iY?]nP_&3uv?0>gx.:V+=R*ue`e$$iSX(u4[_&tw'GMo^,2v8Tr.MuGVPMt;)pLGBm6#uo)M-vOE3MkaamL"
"5UW,#oQ,W-d-NX(tb2v?vkM;@mlQ8fpYAM9B7n;@bX^CsqHU2:>wRe$s-X_&%FX_&T@CL,u4[_&.c[_&$4(GMXNJ#M'G;pLNo'*#31#O-LMo7M6-#O-vn,D-wZ`=-T%kB-@-Vl-V)Ik="
"$0i+Mnbj,#3``=-VBrP-ivgK-+G40M#`j,#QatV@J@On-0)Q9iQD^9iLge?gw;Ck=DTdo@x'/s@d*9JrqN$j:kZ+s@2DO_&$6>s@()RX(jX'GMmN-T-eR-T-r3&N/7Gx1vGo-qLCMI&M"
"6QYO-;CdD-HG`t-YnsRM+2tuLO5.;#1nLAMWh?lLfLL2#V*d%.t&mlL(ML2#dl(T.]&6;#=xX?-;rX?-k$)t-Fn:*M&A2pLYU34#/mG<-/Zwm/.gO2vR+85#.UGs-&A<jLDbp5#5%,s/"
"W7-$vrPo5##q9W.bt2%v;gG<-t-A>-_YGs-<:@#M=Lb%v?Bg;--(7*.Hj@NMsPj(vZ6)=--mG<-LwX?-5;)=-*Hg;-j;)=-.)A>-;[]F-;[]F-;[]F-;[]F-<exb-pX+RE%4l+M9OJ#M"
")^g5#VZkR-MmA,M;b+ZMce#6#;$?8]3`xb-pX+REP<N9imR-F.;$?8]3`xb-pX+RE:o(RE@N/P]x_U2U<vli'gd-p]iJ^e$#Q:-m35W]4+P8R*8Xq.C]c-^5<lLe$k9PX(VhR+`*&C#?"
"(/Le$,g5@0Qnk34vq`q;vq`q;wt`q;wt`q;@6MR*wt`q;)@l+MH'H6#v<LP^3%lDF^?l+MH'H6#v<LP^aPRq;$I1wp7=4]bt:]e$fu+XCE:5;6EYu%cNnr%cOt%&c3[`=-Na`=-r+TV-"
"tNNL#Ro[*#DHc>#H.+*$b<i($fIfT$u&)K$p)ZR$=g^Y$b_?0%>7;O%V_'f%uUv$&q1i(&$o'N&[A0>&r;<l&_Dns&-?C7'#nfE'/O;W'8BYb'J0At'Nn^F(Wh#?(q99Y(+e,v(ZxL,)"
"+<d_)[MLZ)]_9f)J'Cn)GT_v)+g'3**9?G*KZ`w*t.Cr*Okp'+r_j9+vgwH+`a.t+iT@l+##$,,=;_V,<Vdk,JU1g,1&Ax,$T9/-]32<-he``-KIed-Tx+j-NQtm-Kk(8.GA_v-<w32."
"0YAQ.+HF:.GcFB.[.PJ.N^BN.FXun.<)LV./X>Z.UG)d.+5;w.g)8E/nEuj/j`Aq/k<j50$=E=01#MS0.D5v0+paI1ajQ@1p>jT1^1Zg16S@t1XMe12Vj*q2NfLo2,,Mw2I=@53f.0J3"
"AWZw3Mkkb3DPpf3t2v,4]S*241VlZ46oYG4q$FU4TVuv4nK^j4bd;95BqG05VcCW5Op0c5J1M`5AmQd5:X`h51>el56NSq5.=t:6$p]#6qTb'6?pb/6jOnT6KC%A6iUEM6DA&Y6C/<a6"
"M8Lg6/(:,7QePK729`D7*Vqk7p?W*8KfBC8;A/>8;$###5QUA4#vI-##O>+#)/5##c>$(#`G:;$oG&(MH<xu-UIA_MjskZ-Bx1I$&^g;-MJL@-$f`=-(fG<-urG<-^IwA-&sG<-6X`=-"
"YA`T.4l:$#]sG<-krZC=%;Z;%@_2QT'5>>#r(:kFqb+Vd-4$Z$'aDMh.=?v$pBIYm/FZ;%&Z:Mq0OvV%)lkA#)lG<-io#-.BJQ*NS@-##Prk8N((^fL%$A:NUL?##hqj;NUOH##?VD:2"
"eaZ`*7$ZR*8l4R*bUj_&W5w4AOtW_&;pxQELl3DW3krS&8Nb2(e_[_&@4GR*=:]_&6#V_&I_]_&6sC_&-Hf+i;^K/)9Z9j(5WYO-TQx>-,b`=-=x(t-i=FGM<KihL`B8w#;lG<-@ocW-"
"I]g-QCJV_&bBHR*^)9F.dUW_&GVV_&p$X_&nY9F.%CO_&x>@kX-vHQM'w,D-?jG<-Ab`=-[J>HM4G`($<G[/G_wusB#]HKFnMW<BnX@AY<i:D50a`kD1+S+H86XG-dnr.G05DEHCVIr1"
"^DxkEg8*7D>ZOG-_GOVC+t`9Ca8+;CQJQK='DvlEbM+l4qvZR*j:dn*I#+#H,9v>-%DZdNm7--NU.,dMjQ6L2t$d-M4/rfD8ESE-%O;9CuicdG/iCrC)OZ'#6mk.#XaX.#tGwqLfLK-#"
"UvC$#HN=.#S:>)MBBfu-nxjrL,xc.#9GwM0REX*$vuk2$&Gk$Mco`3$8<A5#FSq/#p&@A-K4S>-0N#<-m>(@-'&(1MQ^G&#B;F&#fiPlLRb6lLHa6+#sg$qL0)m.#^S1H-8t%'.ab7NM"
"q((SM,fipL@@]qLhwc.#w2(@-Q86L-9N#<-OvQx-v/RqLV9CSM8(8qLi(8qL,:(sLs;^-#.d&gL8#%*.YU4oLeL#.#u&@A-e>iYQ&=L`PwI`]PqBS>-YA1)..T3rLgD_kLpx:'#G5O50"
"^>:RN68>j1'Nsjt@#9A'K&+XCQ&H5BH*I>H.qX5BuwiQaTTZEe#D1DEJm%aFUjn,4vi^h&;+S5B$td9D)+2Alb12S*@Keo7Nv@5Bg$B2C*3X^O4xc.#m3$50%Q(d2iJ+j9AoLe$nmm3+"
"st[rHK5i2CV[H]FaQ1s7Ft$^G>'*F.fmXw0Bi]9`I7Y&#T?*1#=F5+#cFNvPfV,mq&;cY#*Guu#,]UV$0u68%47no%8ONP&<h/2'@*gi'DBGJ(HZ(,)Ls_c)P5@D*TMw%+XfW]+](9>,"
"a@pu,eXPV-iq18.m3io.qKIP/ud*20#'bi0'?BJ1+W#,2/pYc232;D37Jr%4;cR]4?%4>5C=ku5GUKV6Kn,87O0do7SHDP8Wa%29[#]i9`;=J:dSt+;hlTc;l.6D<pFm%=t_M]=xw.>>"
"&:fu>*RFV?.k'8@2-_o@6E?PA:^v1B>vViBB88JCFPo+DJiOcDN+1DERCh%FV[H]FZt)>G_6auGcNAVHggx7Ik)YoIoA:PJsYq1KwrQiK%53JL)Mj+M-fJcM1(,DN5@c%O9XC]O=q$>P"
"A3[uPEK<VQIds7RM&ToRjhH;$UVl1To'eV$^1.JUt<*s$fbEcVj$'DWn<^%XrT>]Xvmu=Y$0VuY*TRrZ.m3S[2/k4]6GKl]:`,M^>xc._B:Df_FR%G`Jk[(aN-=`aREt@bE%NxbZv5Yc"
"_8m:dcPMrdgi.Sek+f4foCFlfteBig#+$Jh'CZ+i+[;ci^MkCj36S%k7N4]k;gk=lCA-VmKrDonO4&PoSL]1pXnX.q^3:fqbKqFrfdQ(smD:]b:i(cH%YjBF%qV=B.p>LF,MiEH'0c+$"
":t6%$1O6%$Vo.Y'*g^oDk9iTC3A2eGaPH&Fu@ViFJ(u2vJ;x7]p;o4(>PFg7I>c5GT,)ZV`pD)gtV:H&:2IN](p6W%=5_W**>$p8Sp()-v]PF>IsY]Fm3d3>%b)&=9hTA=9np]=.o%g:"
"_WfM9De*j9@_E/:*j8P:']T;6+PpV67%6s6vRhP8q=L58O*3p7_1jV7/&s20#lX@$F=vE$owk@$:59D$YGM>$=6Nx3YU$/_%5V)^*oJ4_m`U)^52-F%8@)9'&.jB4>JvB$2iI_]O262q"
"04^e-lZ-o7YjNT.jD11:J:xX(uK;.GI^@0U9R^S7&<pC41u$`+HDKT*I@^;SEBRo'KQww'V2tx'r$9/-uH'/-F<x=LA?1.Q[g1IQ(m;mL^12Y75qMB4Ks/##hViS%q`+e#*,si'l@2L)"
"#C)L)6=vK)I7mK)x0dK)pI^J)1na>-IPgJ)ClC`+Ni1D+9m,K)FF_hLQ9MhLR^=I)*.:hL7x@S)eSoq7MRsr$@4b@;]+X@;XoE@;&kj-$7rQG`OG`c`Js>J_JvPf_EK0M^ENBi^jJ^Ub"
"BsEXa<E.[`-nl^_'@Ta^?Mpqbv@Wtami?w`c8($`Yaf&_`N[H`0AEa5JrTBLOIYN^T4kS^*c[Z^hT+,2))P]FT+dV$b>$(#'mk.#]Ga5&tM:m&;lh;$,JlY#wWa`<80uQ#o[(MP(hx;;"
"T/YALZ$6h%qr'D?1W+Q#oJBg%RLOC&HsA0M:0(8MrJ1)&=,jW[xOa/%85O^[Y7t&H63oP'=^XQ'I@:a'V3I-M8Bf9.-W0Z)@94Z))Z;$H(+).?Q8+#HRuMe-H?efP^69k0GIx+HUv:GH"
"N####G@WZ4*p'##";

static const char* GetDefaultCompressedFontDataTTFBase85()
{
    return verdana_compressed_data_base85;
}

#endif // #ifndef IMGUI_DISABLE
