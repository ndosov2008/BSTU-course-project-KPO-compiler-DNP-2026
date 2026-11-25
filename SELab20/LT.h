#pragma once
#define LEXEMA_FIXSIZE 1
#define LT_MAXSIZE 4096
#define LT_TI_NULLIDX 0xffffffff

#define LEX_SHORT 't'
#define LEX_CHAR 't'
#define LEX_STRING 't'
#define LEX_VOID 'w'
#define LEX_ID 'i'
#define LEX_LITERAL 'l'
#define LEX_FUNCTION 'f'
#define LEX_MAIN 'm'
#define LEX_RETURN 'r'
#define LEX_PRINT 'p'
#define LEX_INPUT 'g'
#define LEX_IF 'c'
#define LEX_ELSE 'e'
#define LEX_DO 'o'
#define LEX_WHILE 'u'
#define LEX_SEMICOLON ';'
#define LEX_COMMA ','
#define LEX_LEFTBRACE '{'
#define LEX_BRACELET '}'
#define LEX_LEFTHESIS '('
#define LEX_RIGHTHESIS ')'
#define LEX_ASSIGN '='
#define LEX_ARITHM 'v'     // +, -, *, /, %
#define LEX_CMP 'k'        // <, >, ==, !=, >=, <=
#define LEX_AT '@'         // маркер вызова функции в ПОЛИЗ
#define LEX_FILLER '#'     // заполнитель свободных ячеек

namespace LT
{
    struct Entry
    {
        char lexema[LEXEMA_FIXSIZE];
        int sn;
        int idxTI;
        char op[3]; // реальный знак операции ("+", "%", "==", "<=" и т.д.)
    };

    struct LexTable
    {
        int maxsize;
        int size;
        Entry* table;
    };

    LexTable Create(int size);
    void Add(LexTable& lextable, Entry entry);
    Entry GetEntry(LexTable& lextable, int n);
    void Delete(LexTable& lextable);
};