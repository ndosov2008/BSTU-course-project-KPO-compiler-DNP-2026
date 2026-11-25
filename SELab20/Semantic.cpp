#include "stdafx.h"
#include "Semantic.h"
#include "Error.h"
#include <map>

namespace Semantic
{
    static const int MAX_PARAMS = 8;

    static void checkCall(const LT::LexTable& lt, const IT::IdTable& it,
        const std::map<int, std::vector<IT::IDDATATYPE>>& funcParams, int callPos, int& endParenPos)
    {
        int line = lt.table[callPos].sn;
        int funcIdx = lt.table[callPos].idxTI;
        const IT::Entry& fn = it.table[funcIdx];

        if (fn.idtype != IT::F && fn.idtype != IT::S)
            throw Error::geterrorin(303, line, 0);

        std::vector<IT::IDDATATYPE> argTypes;
        int p = callPos + 2;
        while (p < lt.size && lt.table[p].lexema[0] != LEX_RIGHTHESIS)
        {
            char lx = lt.table[p].lexema[0];
            if (lx == LEX_ID || lx == LEX_LITERAL)
            {
                argTypes.push_back(it.table[lt.table[p].idxTI].iddatatype);
            }
            p++;
        }
        endParenPos = p;

        if (fn.idtype == IT::S)
        {
            if (strcmp(fn.id, "abs") == 0)
            {
                if (argTypes.size() != 1) throw Error::geterrorin(312, line, 0);
                if (argTypes[0] != IT::SHORT) throw Error::geterrorin(310, line, 0);
            }
            else if (strcmp(fn.id, "strcmp") == 0)
            {
                if (argTypes.size() != 2) throw Error::geterrorin(312, line, 0);
                if (argTypes[0] != IT::STR || argTypes[1] != IT::STR) throw Error::geterrorin(310, line, 0);
            }
            return;
        }

        if ((int)argTypes.size() > MAX_PARAMS)
            throw Error::geterrorin(307, line, 0);

        auto itSig = funcParams.find(funcIdx);
        if (itSig == funcParams.end())
            throw Error::geterrorin(303, line, 0);

        const auto& expected = itSig->second;
        if (argTypes.size() != expected.size())
            throw Error::geterrorin(308, line, 0);

        for (size_t k = 0; k < expected.size(); k++)
        {
            if (argTypes[k] != expected[k])
                throw Error::geterrorin(310, line, 0);
        }
    }

    static IT::IDDATATYPE evaluateExprType(const LT::LexTable& lt, const IT::IdTable& it,
        const std::map<int, std::vector<IT::IDDATATYPE>>& funcParams, int startPos, int endPos)
    {
        bool hasArithm = false;
        IT::IDDATATYPE exprType = IT::VOID;
        bool typeSet = false;

        for (int i = startPos; i < endPos; i++)
        {
            char lx = lt.table[i].lexema[0];
            int line = lt.table[i].sn;

            if (lx == LEX_ARITHM)
            {
                hasArithm = true;
                if ((lt.table[i].op[0] == '/' || lt.table[i].op[0] == '%') && i + 1 < endPos)
                {
                    if (lt.table[i + 1].lexema[0] == LEX_LITERAL)
                    {
                        const IT::Entry& lit = it.table[lt.table[i + 1].idxTI];
                        if (lit.iddatatype == IT::SHORT && lit.value.vshort == 0)
                            throw Error::geterrorin(318, line, 0);
                    }
                }
            }
            else if (lx == LEX_ID)
            {
                IT::IDDATATYPE t = it.table[lt.table[i].idxTI].iddatatype;
                if (i + 1 < endPos && lt.table[i + 1].lexema[0] == LEX_LEFTHESIS)
                {
                    int endParen = i;
                    checkCall(lt, it, funcParams, i, endParen);
                    i = endParen;
                }
                if (!typeSet) { exprType = t; typeSet = true; }
                else if (exprType != t) throw Error::geterrorin(314, line, 0);
            }
            else if (lx == LEX_LITERAL)
            {
                IT::IDDATATYPE t = it.table[lt.table[i].idxTI].iddatatype;
                if (!typeSet) { exprType = t; typeSet = true; }
                else if (exprType != t) throw Error::geterrorin(314, line, 0);
            }
        }

        if (hasArithm && exprType != IT::SHORT)
            throw Error::geterrorin(316, lt.table[startPos].sn, 0);

        return exprType;
    }

    void analyze(const LT::LexTable& lt, const IT::IdTable& it)
    {
        int mainCount = 0;
        for (int i = 0; i < lt.size; i++)
        {
            if (lt.table[i].lexema[0] == LEX_MAIN) mainCount++;
        }
        if (mainCount == 0) throw Error::geterror(301);
        if (mainCount > 1)  throw Error::geterror(302);

        for (int i = 0; i < it.size; i++)
        {
            if (it.table[i].idtype == IT::L && it.table[i].iddatatype == IT::STR && it.table[i].value.vstr.len == 0)
            {
                int line = lt.table[it.table[i].idxfirstLE].sn;
                throw Error::geterrorin(309, line, 0);
            }
        }

        std::map<int, std::vector<IT::IDDATATYPE>> funcParams;
        for (int i = 0; i < lt.size; i++)
        {
            if (lt.table[i].lexema[0] == LEX_FUNCTION && i + 2 < lt.size && lt.table[i + 2].lexema[0] == LEX_ID)
            {
                int funcIdx = lt.table[i + 2].idxTI;
                std::vector<IT::IDDATATYPE> params;
                int p = i + 4;
                while (p < lt.size && lt.table[p].lexema[0] != LEX_RIGHTHESIS)
                {
                    if (lt.table[p].lexema[0] == LEX_ID)
                        params.push_back(it.table[lt.table[p].idxTI].iddatatype);
                    p++;
                }
                if ((int)params.size() > MAX_PARAMS)
                    throw Error::geterrorin(306, lt.table[i].sn, 0);
                funcParams[funcIdx] = params;
            }
        }

        IT::IDDATATYPE currentFuncType = IT::VOID;

        for (int i = 0; i < lt.size; i++)
        {
            char lx = lt.table[i].lexema[0];
            int line = lt.table[i].sn;

            if (lx == LEX_FUNCTION && i + 2 < lt.size)
            {
                if (lt.table[i + 2].lexema[0] == LEX_MAIN)
                    currentFuncType = IT::VOID;
                else if (lt.table[i + 2].lexema[0] == LEX_ID)
                    currentFuncType = it.table[lt.table[i + 2].idxTI].iddatatype;
            }
            else if (lx == LEX_ASSIGN)
            {
                IT::IDDATATYPE lhsType = it.table[lt.table[i - 1].idxTI].iddatatype;
                int endPos = i + 1;
                while (endPos < lt.size && lt.table[endPos].lexema[0] != LEX_SEMICOLON) endPos++;

                IT::IDDATATYPE rhsType = evaluateExprType(lt, it, funcParams, i + 1, endPos);
                if (rhsType != lhsType)
                    throw Error::geterrorin(314, line, 0);
                i = endPos;
            }
            else if (lx == LEX_RETURN)
            {
                if (currentFuncType == IT::VOID)
                    throw Error::geterrorin(315, line, 0);

                int endPos = i + 1;
                while (endPos < lt.size && lt.table[endPos].lexema[0] != LEX_SEMICOLON) endPos++;

                IT::IDDATATYPE retType = evaluateExprType(lt, it, funcParams, i + 1, endPos);
                if (retType != currentFuncType)
                    throw Error::geterrorin(315, line, 0);
                i = endPos;
            }
            else if (lx == LEX_IF || lx == LEX_WHILE)
            {
                int p = i + 2;
                int cmpPos = -1;
                int depth = 1;
                while (p < lt.size && depth > 0)
                {
                    if (lt.table[p].lexema[0] == LEX_LEFTHESIS) depth++;
                    else if (lt.table[p].lexema[0] == LEX_RIGHTHESIS) depth--;
                    else if (lt.table[p].lexema[0] == LEX_CMP && depth == 1) cmpPos = p;
                    if (depth > 0) p++;
                }
                int endCond = p;
                if (cmpPos != -1)
                {
                    IT::IDDATATYPE leftT = evaluateExprType(lt, it, funcParams, i + 2, cmpPos);
                    IT::IDDATATYPE rightT = evaluateExprType(lt, it, funcParams, cmpPos + 1, endCond);
                    if (leftT != IT::SHORT || rightT != IT::SHORT)
                        throw Error::geterrorin(317, line, 0);
                }
                i = endCond;
            }
            else if (lx == LEX_PRINT)
            {
                int p = i + 2;
                int depth = 1;
                while (p < lt.size && depth > 0)
                {
                    if (lt.table[p].lexema[0] == LEX_LEFTHESIS) depth++;
                    else if (lt.table[p].lexema[0] == LEX_RIGHTHESIS) depth--;
                    if (depth > 0) p++;
                }
                IT::IDDATATYPE printType = evaluateExprType(lt, it, funcParams, i + 2, p);
                if (printType == IT::VOID)
                    throw Error::geterrorin(314, line, 0);
                i = p;
            }
            else if (lx == LEX_ID && i + 1 < lt.size && lt.table[i + 1].lexema[0] == LEX_LEFTHESIS)
            {
                bool isDecl = (i >= 2 && lt.table[i - 2].lexema[0] == LEX_FUNCTION);
                if (!isDecl)
                {
                    int endParen = i;
                    checkCall(lt, it, funcParams, i, endParen);
                    i = endParen;
                }
            }
        }
    }
}