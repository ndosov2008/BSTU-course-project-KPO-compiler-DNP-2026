#pragma once
#include "LT.h"
#include "IT.h"

namespace CodeGen
{
    void generate(const LT::LexTable& lt, const IT::IdTable& it, const wchar_t* outFile);
}