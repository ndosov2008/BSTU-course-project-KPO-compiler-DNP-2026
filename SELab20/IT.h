#pragma once
#define ID_MAXSIZE 17          // до 16 символов + '\0'
#define TI_MAXSIZE 4096
#define TI_SHORT_DEFAULT 0
#define TI_CHAR_DEFAULT '\0'
#define TI_STR_DEFAULT 0x00
#define TI_NULLIDX 0xffffffff
#define TI_STR_MAXSIZE 255

namespace IT
{
    enum IDDATATYPE { SHORT = 1, CHAR = 2, STR = 3, VOID = 4 };
    enum IDTYPE { V = 1, F = 2, P = 3, L = 4, S = 5 }; // S - стандартная библиотека

    struct Entry
    {
        int idxfirstLE;
        char id[ID_MAXSIZE];
        char scope[ID_MAXSIZE]; // область видимости (имя функции)
        IDDATATYPE iddatatype;
        IDTYPE idtype;
        union
        {
            short vshort;
            char vchar;
            struct
            {
                unsigned char len;
                char str[TI_STR_MAXSIZE];
            } vstr;
        } value;
    };

    struct IdTable
    {
        int maxsize;
        int size;
        Entry* table;
    };

    IdTable Create(int size);
    void Add(IdTable& idtable, Entry entry);
    Entry GetEntry(IdTable& idtable, int n);
    int IsId(IdTable& idtable, const char* id, const char* scope = "");
    void Delete(IdTable& idtable);
};