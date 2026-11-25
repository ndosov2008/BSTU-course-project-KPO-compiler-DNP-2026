#pragma once
#define IN_MAX_LEN_TEXT 1024*1024      // максимальный размер исходного кода = 1MB
#define IN_CODE_ENDL '\n'              // символ конца строки

// Таблица проверки входных символов (Windows-1251)
// Разрешены: латиница (A-Z, a-z), цифры, кириллица (вкл. Ё/ё), кавычки " и ',
// операции (+ - * / % = < > !), сепараторы (; , ( ) { }), \ . : ?
#define IN_CODE_TABLE { \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F,       ' ',       '|', In::IN::F, In::IN::F, In::IN::I, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::F, In::IN::F, In::IN::T, In::IN::F, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::F, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::F, In::IN::T, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::F, In::IN::T, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::T, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::T, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, In::IN::F, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, \
    In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T, In::IN::T  \
}

namespace In
{
    struct IN
    {
        enum { T = 1024, F = 2048, I = 4096 };
        int size;
        int lines;
        int ignor;
        char* text;
        int code[256];
    };
    IN getin(wchar_t infile[]);
};