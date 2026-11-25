#include "stdafx.h"
#include "LT.h"
#include "Error.h"

namespace LT
{
    LexTable Create(int size)
    {
        if (size > LT_MAXSIZE) size = LT_MAXSIZE;
        LexTable table;
        table.maxsize = size;
        table.size = 0;
        table.table = new Entry[size];
        return table;
    }

    void Add(LexTable& lextable, Entry entry)
    {
        if (lextable.size < lextable.maxsize)
        {
            lextable.table[lextable.size++] = entry;
        }
        else
        {
            throw Error::geterror(201); // переполнение таблицы лексем
        }
    }

    Entry GetEntry(LexTable& lextable, int n)
    {
        if (n >= 0 && n < lextable.size) return lextable.table[n];
        Entry empty = {};
        empty.idxTI = LT_TI_NULLIDX;
        return empty;
    }

    void Delete(LexTable& lextable)
    {
        delete[] lextable.table;
        lextable.table = nullptr;
    }
}