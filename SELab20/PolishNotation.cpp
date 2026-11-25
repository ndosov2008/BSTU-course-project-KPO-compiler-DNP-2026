#include "stdafx.h"
#include "PolishNotation.h"

struct StackItem
{
    LT::Entry entry;
    int priority;
    bool isFunc;
    int argCount;
};

static int getOperatorPriority(char op)
{
    switch (op)
    {
    case '+':
    case '-': return 2;
    case '*':
    case '/':
    case '%': return 3;
    default:  return -1;
    }
}

static void printPolishExpression(const LT::Entry* out, int outSize, const IT::IdTable& idtable)
{
    std::cout << "  Выражение: ";
    for (int i = 0; i < outSize; i++)
    {
        char lex = out[i].lexema[0];
        if (lex == LEX_ID)
        {
            if (out[i].idxTI >= 0 && out[i].idxTI < idtable.size)
                std::cout << idtable.table[out[i].idxTI].id << ' ';
        }
        else if (lex == LEX_LITERAL)
        {
            if (out[i].idxTI >= 0 && out[i].idxTI < idtable.size)
            {
                const IT::Entry& ie = idtable.table[out[i].idxTI];
                if (ie.iddatatype == IT::SHORT)      std::cout << ie.value.vshort << ' ';
                else if (ie.iddatatype == IT::CHAR)  std::cout << '\'' << ie.value.vchar << "' ";
                else if (ie.iddatatype == IT::STR)   std::cout << '"' << ie.value.vstr.str << "\" ";
            }
        }
        else if (lex == LEX_ARITHM)
        {
            std::cout << out[i].op[0] << ' ';
        }
        else if (lex == LEX_AT)
        {
            std::cout << '@';
            if (out[i].idxTI >= 0 && out[i].idxTI < idtable.size)
                std::cout << idtable.table[out[i].idxTI].id;
            if (i + 1 < outSize && isdigit((unsigned char)out[i + 1].lexema[0]))
            {
                std::cout << '(' << out[i + 1].lexema[0] << ") ";
                i++;
            }
        }
        else
        {
            std::cout << lex << ' ';
        }
    }
    std::cout << std::endl;
}

bool PolishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable)
{
    if (lextable_pos < 0 || lextable_pos >= lextable.size) return false;

    int endPos = lextable_pos;
    while (endPos < lextable.size && lextable.table[endPos].lexema[0] != LEX_SEMICOLON)
        endPos++;

    if (endPos >= lextable.size || endPos == lextable_pos) return false;

    int totalLen = endPos - lextable_pos + 1;
    std::vector<LT::Entry> out;
    std::vector<StackItem> opStack;
    bool ok = true;

    for (int i = lextable_pos; i < endPos && ok; i++)
    {
        LT::Entry cur = lextable.table[i];
        char lex = cur.lexema[0];

        if (lex == LEX_ID && (i + 1 < endPos) && lextable.table[i + 1].lexema[0] == LEX_LEFTHESIS)
        {
            int initArgs = (i + 2 < endPos && lextable.table[i + 2].lexema[0] == LEX_RIGHTHESIS) ? 0 : 1;
            opStack.push_back({ cur, 0, true, initArgs });
            i++;
        }
        else if (lex == LEX_ID || lex == LEX_LITERAL)
        {
            out.push_back(cur);
        }
        else if (lex == LEX_LEFTHESIS)
        {
            opStack.push_back({ cur, 0, false, 0 });
        }
        else if (lex == LEX_COMMA)
        {
            while (!opStack.empty() && opStack.back().priority > 0)
            {
                out.push_back(opStack.back().entry);
                opStack.pop_back();
            }
            if (opStack.empty() || !opStack.back().isFunc) { ok = false; break; }
            opStack.back().argCount++;
        }
        else if (lex == LEX_RIGHTHESIS)
        {
            while (!opStack.empty() && opStack.back().priority > 0)
            {
                out.push_back(opStack.back().entry);
                opStack.pop_back();
            }
            if (opStack.empty()) { ok = false; break; }

            StackItem openItem = opStack.back();
            opStack.pop_back();
            if (openItem.isFunc)
            {
                if (openItem.argCount > 9) { ok = false; break; }

                LT::Entry atEntry = {};
                atEntry.lexema[0] = LEX_AT;
                atEntry.sn = openItem.entry.sn;
                atEntry.idxTI = openItem.entry.idxTI;
                out.push_back(atEntry);

                LT::Entry cntEntry = {};
                cntEntry.lexema[0] = (char)('0' + openItem.argCount);
                cntEntry.sn = openItem.entry.sn;
                cntEntry.idxTI = LT_TI_NULLIDX;
                out.push_back(cntEntry);
            }
        }
        else if (lex == LEX_ARITHM)
        {
            int curPrio = getOperatorPriority(cur.op[0]);
            if (curPrio < 0) { ok = false; break; }

            while (!opStack.empty() && opStack.back().priority >= curPrio)
            {
                out.push_back(opStack.back().entry);
                opStack.pop_back();
            }
            opStack.push_back({ cur, curPrio, false, 0 });
        }
        else
        {
            ok = false;
            break;
        }
    }

    while (ok && !opStack.empty())
    {
        if (opStack.back().priority == 0) { ok = false; break; }
        out.push_back(opStack.back().entry);
        opStack.pop_back();
    }

    if (!ok || out.empty() || (int)out.size() + 1 > totalLen) return false;

    out.push_back(lextable.table[endPos]);

    for (size_t i = 0; i < out.size(); i++)
        lextable.table[lextable_pos + i] = out[i];

    int endLine = lextable.table[endPos].sn;
    for (int i = lextable_pos + (int)out.size(); i <= endPos; i++)
    {
        lextable.table[i] = {};
        lextable.table[i].lexema[0] = LEX_FILLER;
        lextable.table[i].sn = endLine;
        lextable.table[i].idxTI = LT_TI_NULLIDX;
    }

    printPolishExpression(out.data(), (int)out.size(), idtable);
    return true;
}

void ConvertAllExpressions(LT::LexTable& lextable, IT::IdTable& idtable)
{
    std::cout << "\n---- Преобразование выражений в ПОЛИЗ ----" << std::endl;
    for (int i = 0; i < lextable.size; i++)
    {
        char lex = lextable.table[i].lexema[0];
        if (lex == LEX_ASSIGN || lex == LEX_RETURN || lex == LEX_PRINT)
        {
            int exprPos = i + 1;
            std::cout << "Строка " << lextable.table[exprPos].sn << ":" << std::endl;
            PolishNotation(exprPos, lextable, idtable);
        }
    }
}