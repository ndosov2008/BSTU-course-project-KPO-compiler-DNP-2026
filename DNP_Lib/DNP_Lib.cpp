#include "pch.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <windows.h>

extern "C"
{
    void __stdcall outnum(int value)
    {
        SetConsoleOutputCP(1251);
        char buf[32] = {};
        sprintf_s(buf, sizeof(buf), "%d", (short)value);
        DWORD written = 0;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)strlen(buf), &written, nullptr);
    }

    void __stdcall outchar(int value)
    {
        SetConsoleOutputCP(1251);
        char ch = (char)value;
        DWORD written = 0;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), &ch, 1, &written, nullptr);
    }

    void __stdcall outstr(const char* ptr)
    {
        SetConsoleOutputCP(1251);
        if (!ptr) return;
        DWORD written = 0;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), ptr, (DWORD)strlen(ptr), &written, nullptr);
    }

    short __stdcall innum()
    {
        SetConsoleCP(1251);
        char buf[64] = {};
        DWORD readCount = 0;
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buf, sizeof(buf) - 1, &readCount, nullptr);
        buf[readCount] = '\0';
        return (short)atoi(buf);
    }

    char __stdcall inchar()
    {
        SetConsoleCP(1251);
        char buf[32] = {};
        DWORD readCount = 0;
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buf, sizeof(buf) - 1, &readCount, nullptr);
        for (DWORD i = 0; i < readCount; i++)
        {
            if (buf[i] != '\r' && buf[i] != '\n' && buf[i] != ' ')
                return buf[i];
        }
        return '\0';
    }

    void __stdcall in_str(char* ptr)
    {
        SetConsoleCP(1251);
        if (!ptr) return;
        char buf[256] = {};
        DWORD readCount = 0;
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buf, sizeof(buf) - 1, &readCount, nullptr);
        buf[readCount] = '\0';
        while (readCount > 0 && (buf[readCount - 1] == '\r' || buf[readCount - 1] == '\n'))
        {
            buf[--readCount] = '\0';
        }
        strcpy_s(ptr, 256, buf);
    }

    short __stdcall myabs(int value)
    {
        short v = (short)value;
        return (v < 0) ? (short)(-v) : v;
    }

    short __stdcall mystrcmp(const char* s2, const char* s1)
    {
        int r = std::strcmp(s1 ? s1 : "", s2 ? s2 : "");
        if (r < 0) return -1;
        if (r > 0) return 1;
        return 0;
    }

    void __stdcall copystr(char* dst, const char* src)
    {
        if (dst && src) strncpy_s(dst, 256, src, 255);
    }
}