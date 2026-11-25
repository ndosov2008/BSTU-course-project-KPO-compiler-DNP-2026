#include "stdafx.h"
#include "FST.h"

namespace FST
{
    RELATION::RELATION(char c, short ns)
    {
        symbol = c;
        nnode = ns;
    }

    NODE::NODE()
    {
        n_relation = 0;
        relations = nullptr;
    }

    NODE::NODE(short n, RELATION rel, ...)
    {
        n_relation = n;
        relations = new RELATION[n];
        va_list args;
        va_start(args, rel);
        relations[0] = rel;
        for (short i = 1; i < n; i++)
        {
            relations[i] = va_arg(args, RELATION);
        }
        va_end(args);
    }

    FST::FST(char* s, short ns, NODE n, ...)
    {
        string = s;
        nstates = ns;
        nodes = new NODE[ns];
        va_list args;
        va_start(args, n);
        nodes[0] = n;
        for (short i = 1; i < ns; i++)
        {
            nodes[i] = va_arg(args, NODE);
        }
        va_end(args);

        rstates = new short[nstates];
        memset(rstates, 0xff, sizeof(short) * nstates);
        rstates[0] = 0;
        position = -1;
    }

    bool execute(FST& fst)
    {
        short* rstates1 = new short[fst.nstates];
        short* rstates2 = new short[fst.nstates];

        for (short i = 0; i < fst.nstates; i++)
        {
            rstates1[i] = -1;
            rstates2[i] = -1;
        }

        short lstring = strlen(fst.string);
        bool step_possible = true;
        rstates1[0] = 0;

        for (fst.position = 0; fst.position < lstring && step_possible; fst.position++)
        {
            step_possible = false;

            for (short i = 0; i < fst.nstates; i++) rstates2[i] = -1;

            for (short i = 0; i < fst.nstates; i++)
            {
                if (rstates1[i] == fst.position)
                {
                    for (short j = 0; j < fst.nodes[i].n_relation; j++)
                    {
                        if (fst.nodes[i].relations[j].symbol == fst.string[fst.position] ||
                            fst.nodes[i].relations[j].symbol == '\1')
                        {
                            rstates2[fst.nodes[i].relations[j].nnode] = fst.position + 1;
                            step_possible = true;
                        }
                    }
                }
            }
            std::swap(rstates1, rstates2);
        }

        fst.rstates = rstates1;
        delete[] rstates2;

        return (fst.rstates[fst.nstates - 1] == lstring);
    }
}