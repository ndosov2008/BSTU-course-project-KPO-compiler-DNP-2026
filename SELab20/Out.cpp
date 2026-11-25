#include "stdafx.h"
#include "Out.h"
#include "Error.h"


namespace Out
{
    void writeOut(wchar_t outfile[], In::IN in)
    {
        std::ofstream file(outfile);
        if (!file.is_open()) throw Error::geterror(112);
        if (in.text != nullptr) file << in.text;
        file.close();
    }
}