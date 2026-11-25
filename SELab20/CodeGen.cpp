#include "stdafx.h"
#include "CodeGen.h"
#include "Error.h"

namespace CodeGen
{
    static std::string asmName(const IT::Entry& e)
    {
        if (e.idtype == IT::L || e.idtype == IT::F || e.idtype == IT::S || e.scope[0] == '\0')
            return std::string(e.id);
        return std::string(e.scope) + "_" + std::string(e.id);
    }

    static std::string escapeAsmString(const char* s)
    {
        std::string res;
        bool inQuotes = false;
        for (int i = 0; s[i] != '\0'; i++)
        {
            unsigned char c = (unsigned char)s[i];
            if (c == '\n')
            {
                if (inQuotes) { res += "\", "; inQuotes = false; }
                res += "13, 10, ";
            }
            else if (c == '\t')
            {
                if (inQuotes) { res += "\", "; inQuotes = false; }
                res += "9, ";
            }
            else if (c == '"')
            {
                if (inQuotes) { res += "\", "; inQuotes = false; }
                res += "34, ";
            }
            else
            {
                if (!inQuotes) { res += "\""; inQuotes = true; }
                res.push_back((char)c);
            }
        }
        if (inQuotes) res += "\", ";
        res += "0";
        return res;
    }

    static void emitPushOperand(std::ofstream& out, const IT::Entry& e)
    {
        std::string name = asmName(e);
        if (e.idtype == IT::L)
        {
            if (e.iddatatype == IT::SHORT)      out << "\tpush " << e.value.vshort << "\n";
            else if (e.iddatatype == IT::CHAR)  out << "\tpush " << (int)(unsigned char)e.value.vchar << "\n";
            else if (e.iddatatype == IT::STR)   out << "\tpush offset " << name << "\n";
        }
        else if (e.idtype == IT::P)
        {
            out << "\tpush " << name << "\n";
        }
        else
        {
            if (e.iddatatype == IT::SHORT)
            {
                out << "\tmovsx eax, " << name << "\n";
                out << "\tpush eax\n";
            }
            else if (e.iddatatype == IT::CHAR)
            {
                out << "\tmovzx eax, " << name << "\n";
                out << "\tpush eax\n";
            }
            else if (e.iddatatype == IT::STR)
            {
                out << "\tpush offset " << name << "\n";
            }
        }
    }

    static void emitPolishExpr(std::ofstream& out, const LT::LexTable& lt, const IT::IdTable& it, int startPos, int endPos)
    {
        for (int i = startPos; i < endPos; i++)
        {
            char lx = lt.table[i].lexema[0];
            if (lx == LEX_FILLER || lx == LEX_SEMICOLON) continue;

            if (lx == LEX_ID || lx == LEX_LITERAL)
            {
                emitPushOperand(out, it.table[lt.table[i].idxTI]);
            }
            else if (lx == LEX_ARITHM)
            {
                char op = lt.table[i].op[0];
                out << "\tpop ebx\n";
                out << "\tpop eax\n";
                if (op == '+')      out << "\tadd eax, ebx\n";
                else if (op == '-') out << "\tsub eax, ebx\n";
                else if (op == '*') out << "\timul eax, ebx\n";
                else if (op == '/') out << "\tcdq\n\tidiv ebx\n";
                else if (op == '%') out << "\tcdq\n\tidiv ebx\n\tmov eax, edx\n";
                out << "\tpush eax\n";
            }
            else if (lx == LEX_AT)
            {
                const IT::Entry& fn = it.table[lt.table[i].idxTI];
                std::string callName = fn.id;
                if (fn.idtype == IT::S)
                {
                    if (callName == "abs") callName = "myabs";
                    else if (callName == "strcmp") callName = "mystrcmp";
                }
                out << "\tcall " << callName << "\n";
                if (fn.iddatatype != IT::VOID)
                {
                    if (fn.iddatatype == IT::SHORT) out << "\tmovsx eax, ax\n";
                    else if (fn.iddatatype == IT::CHAR) out << "\tmovzx eax, al\n";
                    out << "\tpush eax\n";
                }
                i++;
            }
        }
    }

    static std::string jumpIfFalse(const char* op)
    {
        if (strcmp(op, "<") == 0)  return "jge";
        if (strcmp(op, ">") == 0)  return "jle";
        if (strcmp(op, "<=") == 0) return "jg";
        if (strcmp(op, ">=") == 0) return "jl";
        if (strcmp(op, "==") == 0) return "jne";
        if (strcmp(op, "!=") == 0) return "je";
        return "jmp";
    }

    static std::string jumpIfTrue(const char* op)
    {
        if (strcmp(op, "<") == 0)  return "jl";
        if (strcmp(op, ">") == 0)  return "jg";
        if (strcmp(op, "<=") == 0) return "jle";
        if (strcmp(op, ">=") == 0) return "jge";
        if (strcmp(op, "==") == 0) return "je";
        if (strcmp(op, "!=") == 0) return "jne";
        return "jmp";
    }

    void generate(const LT::LexTable& lt, const IT::IdTable& it, const wchar_t* outFile)
    {
        std::ofstream out(outFile, std::ios::out | std::ios::trunc);
        if (!out.is_open()) throw Error::geterror(113);

        out << ".586\n";
        out << ".model flat, stdcall\n";
        out << "includelib libucrt.lib\n";
        out << "includelib kernel32.lib\n";
        out << "includelib DNP_Lib.lib\n\n";
        out << "ExitProcess PROTO :DWORD\n";
        out << "outnum PROTO :SDWORD\n";
        out << "outchar PROTO :DWORD\n";
        out << "outstr PROTO :DWORD\n";
        out << "innum PROTO\n";
        out << "inchar PROTO\n";
        out << "in_str PROTO :DWORD\n";
        out << "myabs PROTO :SDWORD\n";
        out << "mystrcmp PROTO :DWORD, :DWORD\n";
        out << "copystr PROTO :DWORD, :DWORD\n\n";
        out << ".stack 4096\n\n";

        out << ".const\n";
        for (int i = 0; i < it.size; i++)
        {
            if (it.table[i].idtype == IT::L && it.table[i].iddatatype == IT::STR)
            {
                out << "\t" << it.table[i].id << " BYTE " << escapeAsmString(it.table[i].value.vstr.str) << "\n";
            }
        }
        out << "\n";

        out << ".data\n";
        for (int i = 0; i < it.size; i++)
        {
            if (it.table[i].idtype == IT::V)
            {
                std::string name = asmName(it.table[i]);
                if (it.table[i].iddatatype == IT::SHORT)
                    out << "\t" << name << " SWORD 0\n";
                else if (it.table[i].iddatatype == IT::CHAR)
                    out << "\t" << name << " BYTE 0\n";
                else if (it.table[i].iddatatype == IT::STR)
                    out << "\t" << name << " BYTE 256 DUP(0)\n";
            }
        }
        out << "\n.code\n\n";

        int labelCounter = 0;
        std::string currentFunc;
        bool inMain = false;

        struct BlockInfo { char type; int labelId; bool hasElse; };
        std::vector<BlockInfo> blocks;

        for (int i = 0; i < lt.size; i++)
        {
            char lx = lt.table[i].lexema[0];

            if (lx == LEX_FUNCTION)
            {
                if (lt.table[i + 2].lexema[0] == LEX_MAIN)
                {
                    currentFunc = "main";
                    inMain = true;
                    out << "main PROC\n";
                    while (i < lt.size && lt.table[i].lexema[0] != LEX_LEFTBRACE) i++;
                    blocks.push_back({ 'f', 0, false });
                }
                else
                {
                    int fIdx = lt.table[i + 2].idxTI;
                    currentFunc = it.table[fIdx].id;
                    inMain = false;
                    out << currentFunc << " PROC";

                    int p = i + 4;
                    bool firstParam = true;
                    while (p < lt.size && lt.table[p].lexema[0] != LEX_RIGHTHESIS)
                    {
                        if (lt.table[p].lexema[0] == LEX_ID)
                        {
                            out << (firstParam ? " " : ", ") << asmName(it.table[lt.table[p].idxTI]) << ":SDWORD";
                            firstParam = false;
                        }
                        p++;
                    }
                    out << "\n";
                    while (i < lt.size && lt.table[i].lexema[0] != LEX_LEFTBRACE) i++;
                    blocks.push_back({ 'f', 0, false });
                }
            }
            else if (lx == LEX_ASSIGN)
            {
                const IT::Entry& lhs = it.table[lt.table[i - 1].idxTI];
                int endPos = i + 1;
                while (endPos < lt.size && lt.table[endPos].lexema[0] != LEX_SEMICOLON) endPos++;

                emitPolishExpr(out, lt, it, i + 1, endPos);

                std::string name = asmName(lhs);
                if (lhs.iddatatype == IT::SHORT)
                {
                    out << "\tpop eax\n";
                    out << "\tmov " << name << ", ax\n";
                }
                else if (lhs.iddatatype == IT::CHAR)
                {
                    out << "\tpop eax\n";
                    out << "\tmov " << name << ", al\n";
                }
                else if (lhs.iddatatype == IT::STR)
                {
                    out << "\tpush offset " << name << "\n";
                    out << "\tcall copystr\n";
                }
                i = endPos;
            }
            else if (lx == LEX_PRINT)
            {
                int endPos = i + 1;
                while (endPos < lt.size && lt.table[endPos].lexema[0] != LEX_SEMICOLON) endPos++;

                IT::IDDATATYPE pType = IT::SHORT;
                for (int k = i + 1; k < endPos; k++)
                {
                    if (lt.table[k].lexema[0] == LEX_ID || lt.table[k].lexema[0] == LEX_LITERAL)
                    {
                        pType = it.table[lt.table[k].idxTI].iddatatype;
                        break;
                    }
                }

                emitPolishExpr(out, lt, it, i + 1, endPos);
                if (pType == IT::SHORT)      out << "\tcall outnum\n";
                else if (pType == IT::CHAR)  out << "\tcall outchar\n";
                else if (pType == IT::STR)   out << "\tcall outstr\n";
                i = endPos;
            }
            else if (lx == LEX_INPUT)
            {
                const IT::Entry& var = it.table[lt.table[i + 2].idxTI];
                std::string name = asmName(var);
                if (var.iddatatype == IT::SHORT)
                {
                    out << "\tcall innum\n";
                    out << "\tmov " << name << ", ax\n";
                }
                else if (var.iddatatype == IT::CHAR)
                {
                    out << "\tcall inchar\n";
                    out << "\tmov " << name << ", al\n";
                }
                else if (var.iddatatype == IT::STR)
                {
                    out << "\tpush offset " << name << "\n";
                    out << "\tcall in_str\n";
                }
                while (i < lt.size && lt.table[i].lexema[0] != LEX_SEMICOLON) i++;
            }
            else if (lx == LEX_RETURN)
            {
                int endPos = i + 1;
                while (endPos < lt.size && lt.table[endPos].lexema[0] != LEX_SEMICOLON) endPos++;
                emitPolishExpr(out, lt, it, i + 1, endPos);
                out << "\tpop eax\n";
                out << "\tret\n";
                i = endPos;
            }
            else if (lx == LEX_DO)
            {
                int lbl = ++labelCounter;
                out << "DO_START_" << lbl << ":\n";
                while (i < lt.size && lt.table[i].lexema[0] != LEX_LEFTBRACE) i++;
                blocks.push_back({ 'd', lbl, false });
            }
            else if (lx == LEX_IF)
            {
                int lbl = ++labelCounter;
                const IT::Entry& leftOp = it.table[lt.table[i + 2].idxTI];
                const char* cmpOp = lt.table[i + 3].op;
                const IT::Entry& rightOp = it.table[lt.table[i + 4].idxTI];

                emitPushOperand(out, leftOp);
                emitPushOperand(out, rightOp);
                out << "\tpop ebx\n";
                out << "\tpop eax\n";
                out << "\tcmp eax, ebx\n";
                out << "\t" << jumpIfFalse(cmpOp) << " IF_ELSE_" << lbl << "\n";

                while (i < lt.size && lt.table[i].lexema[0] != LEX_LEFTBRACE) i++;
                blocks.push_back({ 'i', lbl, false });
            }
            else if (lx == LEX_ID && i + 1 < lt.size && lt.table[i + 1].lexema[0] == LEX_LEFTHESIS)
            {
                const IT::Entry& fn = it.table[lt.table[i].idxTI];
                std::vector<int> args;
                int p = i + 2;
                while (p < lt.size && lt.table[p].lexema[0] != LEX_RIGHTHESIS)
                {
                    if (lt.table[p].lexema[0] == LEX_ID || lt.table[p].lexema[0] == LEX_LITERAL)
                        args.push_back(lt.table[p].idxTI);
                    p++;
                }
                for (int k = (int)args.size() - 1; k >= 0; k--)
                    emitPushOperand(out, it.table[args[k]]);
                out << "\tcall " << fn.id << "\n";
                while (i < lt.size && lt.table[i].lexema[0] != LEX_SEMICOLON) i++;
            }
            else if (lx == LEX_BRACELET)
            {
                if (blocks.empty()) continue;
                BlockInfo b = blocks.back();
                blocks.pop_back();

                if (b.type == 'f')
                {
                    if (inMain)
                    {
                        out << "\tpush 0\n";
                        out << "\tcall ExitProcess\n";
                        out << "main ENDP\n";
                        out << "end main\n";
                    }
                    else
                    {
                        out << "\tret\n";
                        out << currentFunc << " ENDP\n\n";
                    }
                }
                else if (b.type == 'd')
                {
                    if (i + 1 < lt.size && lt.table[i + 1].lexema[0] == LEX_WHILE)
                    {
                        const IT::Entry& leftOp = it.table[lt.table[i + 3].idxTI];
                        const char* cmpOp = lt.table[i + 4].op;
                        const IT::Entry& rightOp = it.table[lt.table[i + 5].idxTI];

                        emitPushOperand(out, leftOp);
                        emitPushOperand(out, rightOp);
                        out << "\tpop ebx\n";
                        out << "\tpop eax\n";
                        out << "\tcmp eax, ebx\n";
                        out << "\t" << jumpIfTrue(cmpOp) << " DO_START_" << b.labelId << "\n";

                        while (i < lt.size && lt.table[i].lexema[0] != LEX_SEMICOLON) i++;
                    }
                }
                else if (b.type == 'i')
                {
                    if (i + 1 < lt.size && lt.table[i + 1].lexema[0] == LEX_ELSE)
                    {
                        out << "\tjmp IF_END_" << b.labelId << "\n";
                        out << "IF_ELSE_" << b.labelId << ":\n";
                        while (i < lt.size && lt.table[i].lexema[0] != LEX_LEFTBRACE) i++;
                        blocks.push_back({ 'e', b.labelId, true });
                    }
                    else
                    {
                        out << "IF_ELSE_" << b.labelId << ":\n";
                    }
                }
                else if (b.type == 'e')
                {
                    out << "IF_END_" << b.labelId << ":\n";
                }
            }
        }

        out.close();
    }
}