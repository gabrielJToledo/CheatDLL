// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include "Hooks/Minhook/Minhook.h"
#include "Hooks/Hooks_Core/Hooks_Present.h"

void start()
{
    Beep(500, 800);
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Cheat Injetado" << std::endl;
    KieroHooks::Init();
}

void DllThread()
{
    start();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DllThread, hModule, 0, 0));
        break;
    }
    return TRUE;
}