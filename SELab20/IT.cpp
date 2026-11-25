#include "stdafx.h"
#include "IT.h"
#include "Error.h"

namespace IT
{
    IdTable Create(int size)
    {
        if (size > TI_MAXSIZE) size = TI_MAXSIZE;
        IdTable table;
        table.maxsize = size;
        table.size = 0;
        table.table = new Entry[size];
        return table;
    }

    void Add(IdTable& idtable, Entry entry)
    {
        if (idtable.size < idtable.maxsize)
        {
            idtable.table[idtable.size++] = entry;
        }
        else
        {
            throw Error::geterror(202); // переполнение таблицы идентификаторов
        }
    }

    Entry GetEntry(IdTable& idtable, int n)
    {
        if (n >= 0 && n < idtable.size) return idtable.table[n];
        return {};
    }

    int IsId(IdTable& idtable, const char* id, const char* scope)
    {
        for (int i = 0; i < idtable.size; i++)
        {
            if (strcmp(idtable.table[i].id, id) == 0)
            {
                if (idtable.table[i].idtype == IT::F || idtable.table[i].idtype == IT::S ||
                    scope == nullptr || scope[0] == '\0' || strcmp(idtable.table[i].scope, scope) == 0)
                {
                    return i;
                }
            }
        }
        return TI_NULLIDX;
    }

    void Delete(IdTable& idtable)
    {
        delete[] idtable.table;
        idtable.table = nullptr;
    }
}