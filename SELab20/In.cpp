#include "stdafx.h"
#include "In.h"
#include "Error.h"

namespace In
{
    IN getin(wchar_t infile[])
    {
        IN in;
        in.size = 0;
        in.lines = 1;
        in.ignor = 0;
        int col = 1;

        char* text = new char[IN_MAX_LEN_TEXT];
        int code_table[256] = IN_CODE_TABLE;
        memcpy(in.code, code_table, sizeof(code_table));

        std::ifstream file(infile, std::ios::binary);
        if (!file.is_open())
        {
            delete[] text;
            throw Error::geterror(110);
        }

        char c;
        bool spaceFlag = false;
        bool lineStart = true;
        bool inString = false;
        bool inChar = false;
        bool escaped = false;

        while (file.get(c))
        {
            unsigned char uc = (unsigned char)c;
            int code = code_table[uc];

            if (code == ' ') c = ' '; // замена табуляции на пробел

            if (inString)
            {
                if (escaped) escaped = false;
                else if (c == '\\') escaped = true;
                else if (c == '"') inString = false;
            }
            else if (inChar)
            {
                if (escaped) escaped = false;
                else if (c == '\\') escaped = true;
                else if (c == '\'') inChar = false;
            }
            else
            {
                if (c == '"') inString = true;
                else if (c == '\'') inChar = true;
            }

            if (code == IN::T || code == ' ')
            {
                if (c == ' ' && !inString && !inChar)
                {
                    if (!spaceFlag && !lineStart)
                    {
                        text[in.size++] = c;
                        spaceFlag = true;
                    }
                    else
                    {
                        in.ignor++;
                    }
                }
                else
                {
                    text[in.size++] = c;
                    spaceFlag = false;
                    lineStart = false;
                }
            }
            else if (code == '|')
            {
                if (lineStart)
                {
                    in.ignor++;
                }
                else
                {
                    text[in.size++] = '|';
                    lineStart = true;
                    spaceFlag = false;
                }
                in.lines++;
            }
            else if (code == IN::I)
            {
                in.ignor++;
            }
            else if (code == IN::F)
            {
                delete[] text;
                file.close();
                throw Error::geterrorin(111, in.lines, col);
            }

            col++;
            if (c == '\n') col = 1;
        }

        text[in.size] = '\0';
        in.text = text;
        file.close();
        return in;
    }
}