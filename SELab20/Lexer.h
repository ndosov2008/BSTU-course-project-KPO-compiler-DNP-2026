#pragma once
#include "In.h"
#include "LT.h"
#include "IT.h"
namespace Lexer
{
    void lexicalAnalysis(const In::IN& in, LT::LexTable& lextable, IT::IdTable& idtable);
}