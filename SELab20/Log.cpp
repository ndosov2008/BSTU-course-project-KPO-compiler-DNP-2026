#include "stdafx.h"
#include "Log.h"

namespace Log
{
    LOG getlog(wchar_t logfile[])
    {
        setlocale(LC_ALL, "");
        LOG log = {};
        wcscpy_s(log.logfile, PARM_MAX_SIZE, logfile);
        log.stream = new std::ofstream(logfile, std::ios::out | std::ios::trunc);
        if (!log.stream->is_open())
        {
            delete log.stream;
            log.stream = nullptr;
            throw Error::geterror(112);
        }
        log.isOpen = true;
        return log;
    }

    void WriteLine(LOG log, char* c, ...)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        va_list args;
        va_start(args, c);
        for (char* s = c; s && *s; s = va_arg(args, char*))
            *log.stream << s;
        va_end(args);
        *log.stream << std::endl;
        log.stream->flush();
    }

    void WriteLine(LOG log, wchar_t* c, ...)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        va_list args;
        va_start(args, c);
        for (wchar_t* s = c; s && *s; s = va_arg(args, wchar_t*))
        {
            size_t size_needed = 0;
            if (wcstombs_s(&size_needed, nullptr, 0, s, _TRUNCATE) == 0 && size_needed > 0)
            {
                std::vector<char> buffer(size_needed);
                if (wcstombs_s(&size_needed, buffer.data(), size_needed, s, _TRUNCATE) == 0)
                    *log.stream << buffer.data();
            }
        }
        va_end(args);
        *log.stream << std::endl;
        log.stream->flush();
    }

    void WriteLog(LOG log)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        char datebuf[80];
        strftime(datebuf, sizeof(datebuf), "%d.%m.%Y %H:%M:%S", &timeinfo);

        *log.stream << "---- Протокол работы транслятора DNP-2026 ----\n"
            << "Дата и время: " << datebuf << "\n\n";
        log.stream->flush();
    }

    void WriteParm(LOG log, Parm::PARM parm)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        char in_buf[1024], out_buf[1024], log_buf[1024];
        size_t conv = 0;
        wcstombs_s(&conv, in_buf, sizeof(in_buf), parm.in, _TRUNCATE);
        wcstombs_s(&conv, out_buf, sizeof(out_buf), parm.out, _TRUNCATE);
        wcstombs_s(&conv, log_buf, sizeof(log_buf), parm.log, _TRUNCATE);

        *log.stream << "---- Входные параметры ----\n"
            << "-in:  " << in_buf << "\n"
            << "-out: " << out_buf << "\n"
            << "-log: " << log_buf << "\n\n";
        log.stream->flush();
    }

    void WriteIn(LOG log, In::IN in)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        *log.stream << "---- Результаты ввода исходного кода ----\n"
            << "Количество строк:       " << in.lines << "\n"
            << "Количество символов:    " << in.size << "\n"
            << "Количество пропущенных: " << in.ignor << "\n\n";
        log.stream->flush();
    }

    void WriteLexTable(LOG log, LT::LexTable& lextable)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        *log.stream << "---- Таблица лексем (LT) ----\n";
        int currentLine = -1;
        for (int i = 0; i < lextable.size; ++i)
        {
            if (lextable.table[i].sn != currentLine)
            {
                if (currentLine != -1) *log.stream << "\n";
                currentLine = lextable.table[i].sn;
                char num[16];
                sprintf_s(num, "%02d ", currentLine);
                *log.stream << num;
            }
            *log.stream << lextable.table[i].lexema[0];
        }
        *log.stream << "\n\n";
        log.stream->flush();
    }

    void WriteIdTable(LOG log, IT::IdTable& idtable)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        *log.stream << "---- Таблица идентификаторов (IT) ----\n";
        char header[256];
        sprintf_s(header, " %-3s | %-16s | %-12s | %-7s | %-4s | %s",
            "№", "Имя", "Область", "Тип", "Вид", "Значение");
        *log.stream << header << "\n";

        for (int i = 0; i < idtable.size; ++i)
        {
            const IT::Entry& e = idtable.table[i];
            const char* dataStr = "?";
            switch (e.iddatatype)
            {
            case IT::SHORT: dataStr = "short";  break;
            case IT::CHAR:  dataStr = "char";   break;
            case IT::STR:   dataStr = "string"; break;
            case IT::VOID:  dataStr = "void";   break;
            }

            const char* kindStr = "?";
            switch (e.idtype)
            {
            case IT::V: kindStr = "V"; break;
            case IT::F: kindStr = "F"; break;
            case IT::P: kindStr = "P"; break;
            case IT::L: kindStr = "L"; break;
            case IT::S: kindStr = "S"; break;
            }

            char valbuf[TI_STR_MAXSIZE + 8] = "-";
            if (e.idtype == IT::L || e.idtype == IT::V)
            {
                if (e.iddatatype == IT::SHORT)
                    sprintf_s(valbuf, "%d", e.value.vshort);
                else if (e.iddatatype == IT::CHAR)
                    sprintf_s(valbuf, "'%c'", e.value.vchar ? e.value.vchar : '0');
                else if (e.iddatatype == IT::STR)
                    sprintf_s(valbuf, "\"%s\"", e.value.vstr.str);
            }

            char buf[512];
            sprintf_s(buf, " %-3d | %-16s | %-12s | %-7s | %-4s | %s",
                i, e.id, (e.scope[0] ? e.scope : "-"), dataStr, kindStr, valbuf);
            *log.stream << buf << "\n";
        }
        *log.stream << "\n";
        log.stream->flush();
    }

    void WriteError(LOG log, Error::ERROR err)
    {
        if (!log.isOpen || !log.stream || !log.stream->is_open()) return;
        *log.stream << "Ошибка " << err.id << ": " << err.message << "\n";
        if (err.inext.line != -1)
            *log.stream << "  строка " << err.inext.line << ", позиция " << err.inext.col << "\n";
        log.stream->flush();
    }

    void Close(LOG log)
    {
        if (log.isOpen && log.stream)
        {
            if (log.stream->is_open()) log.stream->close();
            delete log.stream;
        }
    }
}