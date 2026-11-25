#include "stdafx.h"
#include "GRB.h"

#define GRB_ERROR_SERIES 600

namespace GRB
{
#define NS(n) Rule::Chain::N(n)
#define TS(n) Rule::Chain::T(n)

    Greibach greibach(NS('S'), TS('$'), 7,
        Rule(NS('S'), GRB_ERROR_SERIES + 0, 10,
            Rule::Chain(10, TS('f'), TS('t'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')),
            Rule::Chain(9, TS('f'), TS('t'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('}')),
            Rule::Chain(9, TS('f'), TS('t'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')),
            Rule::Chain(8, TS('f'), TS('t'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('}')),
            Rule::Chain(10, TS('f'), TS('w'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')),
            Rule::Chain(9, TS('f'), TS('w'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('}')),
            Rule::Chain(9, TS('f'), TS('w'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')),
            Rule::Chain(8, TS('f'), TS('w'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('}')),
            Rule::Chain(9, TS('f'), TS('w'), TS('m'), TS('('), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')),
            Rule::Chain(8, TS('f'), TS('w'), TS('m'), TS('('), TS(')'), TS('{'), NS('N'), TS('}'))
        ),
        Rule(NS('N'), GRB_ERROR_SERIES + 1, 22,
            Rule::Chain(6, TS('t'), TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
            Rule::Chain(5, TS('t'), TS('i'), TS('='), NS('E'), TS(';')),
            Rule::Chain(4, TS('t'), TS('i'), TS(';'), NS('N')),
            Rule::Chain(3, TS('t'), TS('i'), TS(';')),
            Rule::Chain(5, TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
            Rule::Chain(4, TS('i'), TS('='), NS('E'), TS(';')),
            Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS(';'), NS('N')),
            Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), TS(';')),
            Rule::Chain(5, TS('i'), TS('('), TS(')'), TS(';'), NS('N')),
            Rule::Chain(4, TS('i'), TS('('), TS(')'), TS(';')),
            Rule::Chain(4, TS('r'), NS('E'), TS(';'), NS('N')),
            Rule::Chain(3, TS('r'), NS('E'), TS(';')),
            Rule::Chain(6, TS('p'), TS('('), NS('E'), TS(')'), TS(';'), NS('N')),
            Rule::Chain(5, TS('p'), TS('('), NS('E'), TS(')'), TS(';')),
            Rule::Chain(6, TS('g'), TS('('), TS('i'), TS(')'), TS(';'), NS('N')),
            Rule::Chain(5, TS('g'), TS('('), TS('i'), TS(')'), TS(';')),
            Rule::Chain(10, TS('o'), TS('{'), NS('N'), TS('}'), TS('u'), TS('('), NS('K'), TS(')'), TS(';'), NS('N')),
            Rule::Chain(9, TS('o'), TS('{'), NS('N'), TS('}'), TS('u'), TS('('), NS('K'), TS(')'), TS(';')),
            Rule::Chain(12, TS('c'), TS('('), NS('K'), TS(')'), TS('{'), NS('N'), TS('}'), TS('e'), TS('{'), NS('N'), TS('}'), NS('N')),
            Rule::Chain(11, TS('c'), TS('('), NS('K'), TS(')'), TS('{'), NS('N'), TS('}'), TS('e'), TS('{'), NS('N'), TS('}')),
            Rule::Chain(8, TS('c'), TS('('), NS('K'), TS(')'), TS('{'), NS('N'), TS('}'), NS('N')),
            Rule::Chain(7, TS('c'), TS('('), NS('K'), TS(')'), TS('{'), NS('N'), TS('}'))
        ),
        Rule(NS('K'), GRB_ERROR_SERIES + 5, 6,
            Rule::Chain(3, TS('i'), TS('k'), NS('E')),
            Rule::Chain(3, TS('l'), TS('k'), NS('E')),
            Rule::Chain(5, TS('('), NS('E'), TS(')'), TS('k'), NS('E')),
            Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS('k'), NS('E')),
            Rule::Chain(4, TS('i'), NS('M'), TS('k'), NS('E')),
            Rule::Chain(4, TS('l'), NS('M'), TS('k'), NS('E'))
        ),
        Rule(NS('E'), GRB_ERROR_SERIES + 2, 10,
            Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), NS('M')),
            Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),
            Rule::Chain(4, TS('i'), TS('('), TS(')'), NS('M')),
            Rule::Chain(3, TS('i'), TS('('), TS(')')),
            Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),
            Rule::Chain(3, TS('('), NS('E'), TS(')')),
            Rule::Chain(2, TS('i'), NS('M')),
            Rule::Chain(1, TS('i')),
            Rule::Chain(2, TS('l'), NS('M')),
            Rule::Chain(1, TS('l'))
        ),
        Rule(NS('M'), GRB_ERROR_SERIES + 2, 2,
            Rule::Chain(3, TS('v'), NS('E'), NS('M')),
            Rule::Chain(2, TS('v'), NS('E'))
        ),
        Rule(NS('F'), GRB_ERROR_SERIES + 3, 2,
            Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('F')),
            Rule::Chain(2, TS('t'), TS('i'))
        ),
        Rule(NS('W'), GRB_ERROR_SERIES + 4, 4,
            Rule::Chain(3, TS('i'), TS(','), NS('W')),
            Rule::Chain(3, TS('l'), TS(','), NS('W')),
            Rule::Chain(1, TS('i')),
            Rule::Chain(1, TS('l'))
        )
    );

    Greibach getGreibach() { return greibach; }

    Rule::Chain::Chain(short psize, GRBALPHABET s, ...) {
        size = psize;
        nt = new GRBALPHABET[size];
        va_list args;
        va_start(args, s);
        nt[0] = s;
        for (int i = 1; i < size; i++) nt[i] = va_arg(args, GRBALPHABET);
        va_end(args);
    }

    Rule::Rule(GRBALPHABET pnn, int iderror, short psize, Chain c, ...) {
        nn = pnn; this->iderror = iderror; size = psize;
        chains = new Chain[size];
        va_list args;
        va_start(args, c);
        chains[0] = c;
        for (int i = 1; i < size; i++) chains[i] = va_arg(args, Chain);
        va_end(args);
    }

    Greibach::Greibach(GRBALPHABET pstartN, GRBALPHABET pstbottomT, short psize, Rule r, ...) {
        startN = pstartN; stbottomT = pstbottomT; size = psize;
        rules = new Rule[size];
        va_list args;
        va_start(args, r);
        rules[0] = r;
        for (int i = 1; i < size; i++) rules[i] = va_arg(args, Rule);
        va_end(args);
    }

    short Greibach::getRule(GRBALPHABET pnn, Rule& prule) {
        for (int i = 0; i < size; i++) if (rules[i].nn == pnn) { prule = rules[i]; return i; }
        return -1;
    }

    Rule Greibach::getRule(short n) { return rules[n]; }

    char* Rule::Chain::getCChain(char* b) {
        for (int i = 0; i < size; i++) b[i] = alphabet_to_char(nt[i]);
        b[size] = '\0';
        return b;
    }

    short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j) {
        for (int i = j + 1; i < size; i++) if (chains[i].nt[0] == t) { pchain = chains[i]; return i; }
        return -1;
    }

    char* Rule::getCRule(char* b, short nchain) {
        char buf[256];
        b[0] = Chain::alphabet_to_char(nn); b[1] = '-'; b[2] = '>'; b[3] = '\0';
        strcat_s(b, 256, chains[nchain].getCChain(buf));
        return b;
    }
}