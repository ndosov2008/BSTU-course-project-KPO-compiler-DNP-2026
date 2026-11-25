#include "stdafx.h"
#include "Error.h"
#include "Parm.h"
#include "Log.h"
#include "In.h"
#include "LT.h"
#include "IT.h"
#include "Lexer.h"
#include "MFST.h"
#include "Semantic.h"
#include "PolishNotation.h"
#include "CodeGen.h"

int wmain(int argc, wchar_t* argv[])
{
    setlocale(LC_ALL, "ru");
    Log::LOG log = Log::INITLOG;
    Parm::PARM parm = {};

    try
    {
        parm = Parm::getparm(argc, argv);
        log = Log::getlog(parm.log);
        Log::WriteLog(log);
        Log::WriteParm(log, parm);

        In::IN in = In::getin(parm.in);
        Log::WriteIn(log, in);

        LT::LexTable lextable = LT::Create(LT_MAXSIZE);
        IT::IdTable idtable = IT::Create(TI_MAXSIZE);

        // 1. Лексический анализ
        Lexer::lexicalAnalysis(in, lextable, idtable);
        if (parm.lt) Log::WriteLexTable(log, lextable);
        if (parm.it) Log::WriteIdTable(log, idtable);

        // 2. Синтаксический анализ
        MFST_TRACE_START
            MFST::Mfst mfst(lextable, GRB::getGreibach());
        bool syntaxOk = mfst.start();
        if (!syntaxOk)
        {
            Log::WriteLine(log, (char*)"Синтаксический анализ завершен с ошибками.", (char*)"");
            throw Error::geterror(600);
        }

        // 3. Семантический анализ (Лаба 21)
        Semantic::analyze(lextable, idtable);
        std::cout << "Семантический анализ выполнен без ошибок." << std::endl;

        // 4. Преобразование выражений в ПОЛИЗ (Лаба 20)
        ConvertAllExpressions(lextable, idtable);
        if (parm.pn)
        {
            Log::WriteLine(log, (char*)"---- Таблица лексем после ПОЛИЗ ----", (char*)"");
            Log::WriteLexTable(log, lextable);
        }

        // 5. Генерация кода MASM (Лаба 22)
        CodeGen::generate(lextable, idtable, parm.out);
        std::cout << "Генерация кода MASM успешно завершена." << std::endl;

        LT::Delete(lextable);
        IT::Delete(idtable);
        delete[] in.text;
    }
    catch (Error::ERROR e)
    {
        Log::WriteError(log, e);
        std::cout << "Ошибка " << e.id << ": " << e.message << std::endl;
        if (e.inext.line != -1)
            std::cout << "  строка " << e.inext.line << ", позиция " << e.inext.col << std::endl;
    }

    Log::Close(log);
    system("pause");
    return 0;
}