#include <cstdlib>
#include "pch.h"
#include "helper.h"

void Helper::OpenConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
}

void Helper::CloseConsole()
{
    FreeConsole();
}

void Helper::NopMemory(BYTE* destination, unsigned int size)
{
    DWORD odt;
    VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &odt);
    memset(destination, 0x90, size);
    VirtualProtect(destination, size, odt, &odt);
}

void Helper::PatchMemory(BYTE* destination, BYTE* source, unsigned int size)
{
    DWORD odt;
    VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &odt);
    memcpy(destination, source, size);
    VirtualProtect(destination, size, odt, &odt);
}

uintptr_t Helper::FindM(uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i)
    {
        addr = *(uintptr_t*)addr;
        addr += offsets[i];
    }
    return addr;
}

uintptr_t Helper::PatternScan(uintptr_t moduleAdress, const char* signature)
{
    static auto patternToByte = [](const char* pattern)
    {
        auto       bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else { bytes.push_back(strtoul(current, &current, 16)); }
        }
        return bytes;
    };

    const auto dosHeader = (PIMAGE_DOS_HEADER)moduleAdress;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdress + dosHeader->e_lfanew);

    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto       patternBytes = patternToByte(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(moduleAdress);

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found) { return reinterpret_cast<uintptr_t>(&scanBytes[i]); }
    }
    return NULL;
}