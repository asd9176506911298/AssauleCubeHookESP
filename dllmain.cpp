// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"
#include <Windows.h>
#include "hook.h"
#include "glDraw.h"
#include "gltext.h"
#include "ESP.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

typedef int(__stdcall* twglSwapBuffers)(int a1);

twglSwapBuffers wglSwapBuffersGateWay;

GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;
const char* example = "ESP Box";
const char* example2 = "Yuki";

ESP esp;

void Draw()
{
    HDC currentHDC = wglGetCurrentDC();

    if (!glFont.bBuilt || currentHDC != glFont.hdc)
    {
        glFont.Build(FONT_HEIGHT);
    }

    GL::SetupOrtho();
    esp.Draw(glFont);
    GL::RestoreGL();
}

int __stdcall hkwglSwapBuffers(int a1)
{

    Draw();

    return wglSwapBuffersGateWay(a1);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateWay, 5);

    SwapBuffersHook.Enable();
    
    //Sleep(15000);
    //SwapBuffersHook.Disable();


    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
        if (hThread) CloseHandle(hThread);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

