// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "helper.h"

uintptr_t moduleBaseGameAssembly;
uintptr_t moduleBaseUnityPlayer;
bool bFakePing = false, bFly = false, bDblJump = false;
float zOffset = 0;

DWORD WINAPI VRCHook(HMODULE hModule) {
    Helper::OpenConsole();

    std::cout << "VRC-Hook by bay\n";
    std::cout << "1 - Fake ping\n";
    std::cout << "2 - Fly\n";
    std::cout << "3 - Z add\n";

    moduleBaseGameAssembly = (uintptr_t)GetModuleHandle("GameAssembly.dll");
    moduleBaseUnityPlayer = (uintptr_t)GetModuleHandle("UnityPlayer.dll");

    while (true) {
        uintptr_t* zValuePtr = (uintptr_t*)(moduleBaseUnityPlayer + 0x150CDE4);

        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }

        if (GetAsyncKeyState(49) & 1) {
            bFakePing = !bFakePing;
        }

        if (GetAsyncKeyState(50) & 1) {
            bFly = !bFly;
            if (zValuePtr) {
                zOffset = *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, { 0x58, 0x0, 0x370, 0x10, 0x68, 0x0, 0x1FC });
            }
        }

        if (GetAsyncKeyState(51) & 1) {
            bDblJump = !bDblJump;
        }

        if (bFakePing) {
            *(int*)Helper::FindM(moduleBaseGameAssembly + 0x0612EB60, { 0xC0, 0x0, 0x18, 0xB8, 0x10, 0xA0, 0x6C }) = 9999;
        }

        if (bDblJump) {
            if (GetAsyncKeyState(VK_SPACE) & 1) {
                *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, { 0x58, 0x0, 0x370, 0x10, 0x68, 0x0, 0x1FC }) += 0.5f;
            }
        }

        if (bFly) {
            if (GetAsyncKeyState(0x45) & 1) {
                zOffset += 0.1f;
            }
            else if (GetAsyncKeyState(0x51) & 1) {
                zOffset -= 0.1f;
            }
            if (zValuePtr) {
                *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, { 0x58, 0x0, 0x370, 0x10, 0x68, 0x0, 0x1FC }) = zOffset;
            }
        }
        Sleep(1);
    }

    std::cout << "Unhooked\n";

    Helper::CloseConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)VRCHook, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

