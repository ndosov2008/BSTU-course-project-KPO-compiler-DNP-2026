#include "stdafx.h"
#include "Parm.h"
#include "Error.h"

namespace Parm
{
    PARM getparm(int argc, wchar_t* argv[])
    {
        PARM parm = {};
        bool in_found = false;

        for (int i = 1; i < argc; ++i)
        {
            if (wcsstr(argv[i], PARM_IN) == argv[i])
            {
                const wchar_t* value = argv[i] + wcslen(PARM_IN);
                if (wcslen(value) >= PARM_MAX_SIZE) throw Error::geterror(104);
                wcscpy_s(parm.in, PARM_MAX_SIZE, value);
                in_found = true;
            }
            else if (wcsstr(argv[i], PARM_OUT) == argv[i])
            {
                const wchar_t* value = argv[i] + wcslen(PARM_OUT);
                if (wcslen(value) >= PARM_MAX_SIZE) throw Error::geterror(104);
                wcscpy_s(parm.out, PARM_MAX_SIZE, value);
            }
            else if (wcsstr(argv[i], PARM_LOG) == argv[i])
            {
                const wchar_t* value = argv[i] + wcslen(PARM_LOG);
                if (wcslen(value) >= PARM_MAX_SIZE) throw Error::geterror(104);
                wcscpy_s(parm.log, PARM_MAX_SIZE, value);
            }
            else if (wcscmp(argv[i], PARM_LT) == 0)   parm.lt = true;
            else if (wcscmp(argv[i], PARM_IT) == 0)   parm.it = true;
            else if (wcscmp(argv[i], PARM_TREE) == 0) parm.tree = true;
            else if (wcscmp(argv[i], PARM_PN) == 0)   parm.pn = true;
        }

        if (!in_found) throw Error::geterror(100);

        if (parm.out[0] == L'\0')
        {
            wcscpy_s(parm.out, PARM_MAX_SIZE, parm.in);
            wcscat_s(parm.out, PARM_MAX_SIZE, PARM_OUT_DEFAULT_EXT);
        }
        if (parm.log[0] == L'\0')
        {
            wcscpy_s(parm.log, PARM_MAX_SIZE, parm.in);
            wcscat_s(parm.log, PARM_MAX_SIZE, PARM_LOG_DEFAULT_EXT);
        }

        return parm;
    }
}