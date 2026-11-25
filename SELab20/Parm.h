#pragma once
#define PARM_IN   L"-in:"
#define PARM_OUT  L"-out:"
#define PARM_LOG  L"-log:"
#define PARM_LT   L"-lt"
#define PARM_IT   L"-it"
#define PARM_TREE L"-tree"
#define PARM_PN   L"-pn"
#define PARM_MAX_SIZE 300
#define PARM_OUT_DEFAULT_EXT L".asm"
#define PARM_LOG_DEFAULT_EXT L".log"

namespace Parm
{
    struct PARM
    {
        wchar_t in[PARM_MAX_SIZE];
        wchar_t out[PARM_MAX_SIZE];
        wchar_t log[PARM_MAX_SIZE];
        bool lt;
        bool it;
        bool tree;
        bool pn;
    };

    PARM getparm(int argc, wchar_t* argv[]);
};