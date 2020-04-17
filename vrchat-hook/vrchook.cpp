#include "vrchook.h"

uintptr_t moduleBaseGameAssembly;
uintptr_t moduleBaseUnityPlayer;
bool bFakePing = false, bFly = false, bDblJump = false;
float zOffset = 0;
float coord = 0;
std::vector<unsigned int> pingOffset = { 0xC0, 0x0, 0x18, 0xB8, 0x10, 0xA0, 0x6C };
std::vector<unsigned int> xAxisOffset = { 0x40, 0xC0, 0x30, 0x88, 0x18, 0xC0, 0x2C8 };
std::vector<unsigned int> zAxisOffset = { 0x58, 0x0, 0x370, 0x10, 0x68, 0x0, 0x1FC };



void VRCHook(HMODULE hModule) {
    Helper::OpenConsole();

    std::cout << "VRC-Hook by bay\n";
    std::cout << "1 - Fake ping\n";
    std::cout << "2 - Fly\n";
    std::cout << "3 - Double jump add\n";
    std::cout << "4 - Output X, Z\n";


    moduleBaseGameAssembly = (uintptr_t)GetModuleHandle("GameAssembly.dll");
    moduleBaseUnityPlayer = (uintptr_t)GetModuleHandle("UnityPlayer.dll");

    while (true) {

        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }

        if (GetAsyncKeyState(49) & 1) {
            bFakePing = !bFakePing;
        }

        if (GetAsyncKeyState(50) & 1) {
            bFly = !bFly;
            zOffset = *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, zAxisOffset);
        }

        if (GetAsyncKeyState(51) & 1) {
            bDblJump = !bDblJump;
        }

        if (GetAsyncKeyState(52) & 1) {
            std::cout << "Current X: " << *(float*)Helper::FindM(moduleBaseGameAssembly + 0x06ACCAF8, xAxisOffset) << "\n";
            std::cout << "Current Z: " << *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, zAxisOffset) << "\n";
        }

        if (bFakePing) {
            *(int*)Helper::FindM(moduleBaseGameAssembly + 0x0612EB60, pingOffset) = 9999;
        }

        if (bDblJump) {
            if (GetAsyncKeyState(VK_SPACE) & 1) {
                *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, zAxisOffset) += 0.2f;
            }
        }

        if (bFly) {
            if (GetAsyncKeyState(0x45) & 1) {
                zOffset += 0.1f;
            }
            else if (GetAsyncKeyState(0x51) & 1) {
                zOffset -= 0.1f;
            }
            *(float*)Helper::FindM(moduleBaseUnityPlayer + 0x0150C800, zAxisOffset) = zOffset;
        }
        Sleep(1);
    }

    std::cout << "\nUnhooked\n";

    Helper::CloseConsole();
    FreeLibraryAndExitThread(hModule, 0);
}