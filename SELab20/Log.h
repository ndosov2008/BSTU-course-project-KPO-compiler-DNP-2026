#pragma once
#include <fstream>
#include "In.h" 
#include "Parm.h"
#include "LT.h"
#include "IT.h"
#include "Error.h"       

namespace Log                  
{
    struct LOG                  
    {
        wchar_t logfile[PARM_MAX_SIZE]; 
        std::ofstream* stream;          
        bool isOpen;                    
    };

    static const LOG INITLOG{ L"", nullptr, false };     
    LOG getlog(wchar_t logfile[]);                       
    void WriteLine(LOG log, char* c, ...);               
    void WriteLine(LOG log, wchar_t* c, ...);            
    void WriteLog(LOG log);                              
    void WriteParm(LOG log, Parm::PARM parm);            
    void WriteIn(LOG log, In::IN in);                    
    void WriteError(LOG log, Error::ERROR error);        
    void Close(LOG log);

    void WriteLexTable(LOG log, LT::LexTable& lextable);
    void WriteIdTable(LOG log, IT::IdTable& idtable);
};
