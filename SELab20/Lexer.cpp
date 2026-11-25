#include "stdafx.h"
#include "Lexer.h"
#include "FST.h"
#include "Error.h"

namespace Lexer
{
    static bool matchExactFST(const char* word, const char* pattern)
    {
        int len = (int)strlen(pattern);
        if ((int)strlen(word) != len) return false;

        std::vector<FST::RELATION> rels(len);
        std::vector<FST::NODE> nodes(len + 1);
        for (int i = 0; i < len; i++)
        {
            rels[i] = FST::RELATION(pattern[i], (short)(i + 1));
            nodes[i].n_relation = 1;
            nodes[i].relations = &rels[i];
        }
        nodes[len].n_relation = 0;
        nodes[len].relations = nullptr;

        FST::FST fst((char*)word, 1, FST::NODE());
        delete[] fst.nodes;
        delete[] fst.rstates;
        fst.nstates = (short)(len + 1);
        fst.nodes = nodes.data();
        fst.rstates = nullptr;

        bool res = FST::execute(fst);
        delete[] fst.rstates;
        fst.nodes = nullptr;
        return res;
    }

    static bool isHexDigit(char c)
    {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    static bool isUnaryMinusContext(const LT::LexTable& lt)
    {
        if (lt.size == 0) return true;
        char prev = lt.table[lt.size - 1].lexema[0];
        return prev == '=' || prev == '(' || prev == ',' || prev == LEX_RETURN || prev == LEX_ARITHM || prev == LEX_CMP;
    }

    static void addLiteral(LT::LexTable& lt, IT::IdTable& it, int line, IT::IDDATATYPE type,
        short vshort, char vchar, const char* vstr, int& litCounter)
    {
        LT::Entry le = {};
        le.lexema[0] = LEX_LITERAL;
        le.sn = line;
        le.idxTI = it.size;
        LT::Add(lt, le);

        IT::Entry ie = {};
        ie.idxfirstLE = lt.size - 1;
        ie.iddatatype = type;
        ie.idtype = IT::L;
        ie.scope[0] = '\0';
        sprintf_s(ie.id, ID_MAXSIZE, "L%d", litCounter++);

        if (type == IT::SHORT) ie.value.vshort = vshort;
        else if (type == IT::CHAR) ie.value.vchar = vchar;
        else if (type == IT::STR)
        {
            size_t len = strlen(vstr);
            ie.value.vstr.len = (unsigned char)len;
            strcpy_s(ie.value.vstr.str, TI_STR_MAXSIZE, vstr);
        }
        IT::Add(it, ie);
    }

    void lexicalAnalysis(const In::IN& in, LT::LexTable& lt, IT::IdTable& it)
    {
        int pos = 0, line = 1, col = 1;
        int litCounter = 0;
        IT::IDDATATYPE currentType = IT::SHORT;
        char currentScope[ID_MAXSIZE] = "";
        bool funcPending = false;
        bool expectingParams = false;
        bool inParams = false;
        bool declPending = false;
        int braceDepth = 0;

        while (in.text[pos] != '\0')
        {
            char c = in.text[pos];

            if (c == ' ') { pos++; col++; continue; }
            if (c == '|') { line++; col = 1; pos++; continue; }

            // Составные операторы сравнения: ==, !=, <=, >=
            if ((c == '=' || c == '!' || c == '<' || c == '>') && in.text[pos + 1] == '=')
            {
                LT::Entry le = {};
                le.lexema[0] = LEX_CMP;
                le.sn = line;
                le.idxTI = LT_TI_NULLIDX;
                le.op[0] = c;
                le.op[1] = '=';
                le.op[2] = '\0';
                LT::Add(lt, le);
                pos += 2; col += 2;
                continue;
            }

            // Отрицательный целочисленный литерал
            if (c == '-' && isdigit((unsigned char)in.text[pos + 1]) && isUnaryMinusContext(lt))
            {
                int startCol = col;
                int startPos = pos++;
                col++;
                bool isHex = (in.text[pos] == '0' && (in.text[pos + 1] == 'x' || in.text[pos + 1] == 'X'));
                if (isHex)
                {
                    pos += 2; col += 2;
                    while (isHexDigit(in.text[pos])) { pos++; col++; }
                }
                else
                {
                    while (isdigit((unsigned char)in.text[pos])) { pos++; col++; }
                }
                if (isalpha((unsigned char)in.text[pos])) throw Error::geterrorin(204, line, startCol);

                std::string numStr(in.text + startPos, pos - startPos);
                long long val = std::stoll(numStr, nullptr, isHex ? 16 : 10);
                if (val < -32768 || val > 32767) throw Error::geterrorin(204, line, startCol);

                addLiteral(lt, it, line, IT::SHORT, (short)val, '\0', "", litCounter);
                continue;
            }

            // Одиночные сепараторы и операторы
            if (strchr(";,{}()+-*/%=<>", c))
            {
                LT::Entry le = {};
                le.sn = line;
                le.idxTI = LT_TI_NULLIDX;

                if (c == '(' && expectingParams) { inParams = true; expectingParams = false; }
                else if (c == ')') { inParams = false; }
                else if (c == '{') { braceDepth++; }
                else if (c == '}')
                {
                    braceDepth--;
                    if (braceDepth == 0) currentScope[0] = '\0';
                }

                if (strchr("+-*/%", c))
                {
                    le.lexema[0] = LEX_ARITHM;
                    le.op[0] = c;
                }
                else if (c == '<' || c == '>')
                {
                    le.lexema[0] = LEX_CMP;
                    le.op[0] = c;
                }
                else
                {
                    le.lexema[0] = c;
                }

                LT::Add(lt, le);
                pos++; col++;
                continue;
            }

            // Строковый литерал "..."
            if (c == '"')
            {
                int startCol = col;
                pos++; col++;
                std::string content;
                while (in.text[pos] != '\0' && in.text[pos] != '"' && in.text[pos] != '|')
                {
                    if (in.text[pos] == '\\' && in.text[pos + 1] != '\0')
                    {
                        char next = in.text[pos + 1];
                        if (next == 'n') content.push_back('\n');
                        else if (next == 't') content.push_back('\t');
                        else if (next == '"') content.push_back('"');
                        else if (next == '\\') content.push_back('\\');
                        else content.push_back(next);
                        pos += 2; col += 2;
                    }
                    else
                    {
                        content.push_back(in.text[pos++]);
                        col++;
                    }
                }
                if (in.text[pos] != '"') throw Error::geterrorin(311, line, startCol);
                if (content.size() >= TI_STR_MAXSIZE) throw Error::geterrorin(319, line, startCol);

                addLiteral(lt, it, line, IT::STR, 0, '\0', content.c_str(), litCounter);
                pos++; col++;
                continue;
            }

            // Символьный литерал '...'
            if (c == '\'')
            {
                int startCol = col;
                pos++; col++;
                char ch = '\0';
                if (in.text[pos] == '\\' && in.text[pos + 1] != '\0')
                {
                    char next = in.text[pos + 1];
                    ch = (next == 'n') ? '\n' : (next == '0' ? '\0' : next);
                    pos += 2; col += 2;
                }
                else if (in.text[pos] != '\0' && in.text[pos] != '\'' && in.text[pos] != '|')
                {
                    ch = in.text[pos++];
                    col++;
                }
                else
                {
                    throw Error::geterrorin(204, line, startCol);
                }

                if (in.text[pos] != '\'') throw Error::geterrorin(204, line, startCol);
                addLiteral(lt, it, line, IT::CHAR, 0, ch, "", litCounter);
                pos++; col++;
                continue;
            }

            // Положительный числовой литерал (10-й или 16-й)
            if (isdigit((unsigned char)c))
            {
                int startCol = col;
                int startPos = pos;
                bool isHex = (c == '0' && (in.text[pos + 1] == 'x' || in.text[pos + 1] == 'X'));
                if (isHex)
                {
                    pos += 2; col += 2;
                    if (!isHexDigit(in.text[pos])) throw Error::geterrorin(204, line, startCol);
                    while (isHexDigit(in.text[pos])) { pos++; col++; }
                }
                else
                {
                    while (isdigit((unsigned char)in.text[pos])) { pos++; col++; }
                }
                if (isalpha((unsigned char)in.text[pos])) throw Error::geterrorin(204, line, startCol);

                std::string numStr(in.text + startPos, pos - startPos);
                long long val = std::stoll(numStr, nullptr, isHex ? 16 : 10);
                if (val < -32768 || val > 32767) throw Error::geterrorin(204, line, startCol);

                addLiteral(lt, it, line, IT::SHORT, (short)val, '\0', "", litCounter);
                continue;
            }

            // Ключевые слова и идентификаторы
            if (isalpha((unsigned char)c))
            {
                int startCol = col;
                int startPos = pos;
                while (isalnum((unsigned char)in.text[pos])) { pos++; col++; }
                int len = pos - startPos;
                std::string word(in.text + startPos, len);

                LT::Entry le = {};
                le.sn = line;
                le.idxTI = LT_TI_NULLIDX;

                if (matchExactFST(word.c_str(), "short")) { le.lexema[0] = LEX_SHORT; currentType = IT::SHORT; declPending = true; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "char")) { le.lexema[0] = LEX_CHAR; currentType = IT::CHAR; declPending = true; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "string")) { le.lexema[0] = LEX_STRING; currentType = IT::STR; declPending = true; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "void")) { le.lexema[0] = LEX_VOID; currentType = IT::VOID; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "function")) { le.lexema[0] = LEX_FUNCTION; funcPending = true; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "return")) { le.lexema[0] = LEX_RETURN; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "print")) { le.lexema[0] = LEX_PRINT; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "input")) { le.lexema[0] = LEX_INPUT; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "if")) { le.lexema[0] = LEX_IF; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "else")) { le.lexema[0] = LEX_ELSE; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "do")) { le.lexema[0] = LEX_DO; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "while")) { le.lexema[0] = LEX_WHILE; LT::Add(lt, le); continue; }
                if (matchExactFST(word.c_str(), "main"))
                {
                    le.lexema[0] = LEX_MAIN;
                    strcpy_s(currentScope, ID_MAXSIZE, "main");
                    funcPending = false;
                    expectingParams = true;
                    LT::Add(lt, le);
                    continue;
                }

                if (len > 16) throw Error::geterrorin(203, line, startCol);

                // Функции стандартной библиотеки (abs, strcmp)
                if (word == "abs" || word == "strcmp")
                {
                    int idx = IT::IsId(it, word.c_str(), "");
                    if (idx == TI_NULLIDX)
                    {
                        idx = it.size;
                        IT::Entry ie = {};
                        ie.idxfirstLE = lt.size;
                        strcpy_s(ie.id, ID_MAXSIZE, word.c_str());
                        ie.scope[0] = '\0';
                        ie.iddatatype = IT::SHORT;
                        ie.idtype = IT::S;
                        IT::Add(it, ie);
                    }
                    le.lexema[0] = LEX_ID;
                    le.idxTI = idx;
                    LT::Add(lt, le);
                    continue;
                }

                // Пользовательские функции, параметры, переменные
                if (funcPending)
                {
                    if (IT::IsId(it, word.c_str(), "") != TI_NULLIDX)
                        throw Error::geterrorin(305, line, startCol);

                    le.lexema[0] = LEX_ID;
                    le.idxTI = it.size;
                    LT::Add(lt, le);

                    IT::Entry ie = {};
                    ie.idxfirstLE = lt.size - 1;
                    strcpy_s(ie.id, ID_MAXSIZE, word.c_str());
                    ie.scope[0] = '\0';
                    ie.iddatatype = currentType;
                    ie.idtype = IT::F;
                    IT::Add(it, ie);

                    strcpy_s(currentScope, ID_MAXSIZE, word.c_str());
                    funcPending = false;
                    declPending = false;
                    expectingParams = true;
                    continue;
                }

                if (declPending)
                {
                    if (IT::IsId(it, word.c_str(), currentScope) != TI_NULLIDX)
                        throw Error::geterrorin(305, line, startCol);

                    le.lexema[0] = LEX_ID;
                    le.idxTI = it.size;
                    LT::Add(lt, le);

                    IT::Entry ie = {};
                    ie.idxfirstLE = lt.size - 1;
                    strcpy_s(ie.id, ID_MAXSIZE, word.c_str());
                    strcpy_s(ie.scope, ID_MAXSIZE, currentScope);
                    ie.iddatatype = currentType;
                    ie.idtype = inParams ? IT::P : IT::V;
                    IT::Add(it, ie);

                    declPending = false;
                    continue;
                }

                int idx = IT::IsId(it, word.c_str(), currentScope);
                if (idx == TI_NULLIDX) throw Error::geterrorin(300, line, startCol);

                le.lexema[0] = LEX_ID;
                le.idxTI = idx;
                LT::Add(lt, le);
                continue;
            }

            throw Error::geterrorin(200, line, col);
        }
    }
}