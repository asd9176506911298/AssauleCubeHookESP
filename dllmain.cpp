// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"
#include <Windows.h>
#include "hook.h"
#include "glDraw.h"
#include "gltext.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class Entity
{
public:
    union
    {
        DEFINE_MEMBER_N(int, health, 0xF8);
    };
};

typedef int(__stdcall* twglSwapBuffers)(int a1);

twglSwapBuffers wglSwapBuffersGateWay;

GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;
const char* example = "ESP Box";
const char* example2 = "Yuki";

void Draw()
{
    HDC currentHDC = wglGetCurrentDC();

    if (!glFont.bBuilt || currentHDC != glFont.hdc)
    {
        glFont.Build(FONT_HEIGHT);
    }

    GL::SetupOrtho();

    GL::DrawOutline(412, 284, 200, 200, 2.0f, rgb::red);

    float textPointX = glFont.centerText(412, 200, strlen(example) * FONT_WIDTH);
    float textPointY = 284 - FONT_HEIGHT / 2;

    glFont.Print(textPointX, textPointY, rgb::green, "%s", example);

    vec3 insideTextPoint = glFont.centerText(412, 284 + 100, 200, 200, strlen(example2) * FONT_WIDTH, FONT_HEIGHT);
    glFont.Print(insideTextPoint.x, insideTextPoint.y, rgb::green, "%s", example2);
    
    GL::RestoreGL();
}

int __stdcall hkwglSwapBuffers(int a1)
{
    Entity* ent = *(Entity**)(0x50F4F4);
    if (ent)
    {
        ent->health = 1337;
    }

    Draw();

    return wglSwapBuffersGateWay(a1);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateWay, 5);

    SwapBuffersHook.Enable();
    
    //Sleep(2000);
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

