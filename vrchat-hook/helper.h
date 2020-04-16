#pragma once

#include <vector>
class Helper
{
public:
    static uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature);
    static void NopMemory(BYTE* dst, unsigned int size);
    static void PatchMemory(BYTE* dst, BYTE* src, unsigned int size);
    static uintptr_t FindM(uintptr_t ptr, std::vector<unsigned int> offsets);
    static void OpenConsole();
    static void CloseConsole();
};