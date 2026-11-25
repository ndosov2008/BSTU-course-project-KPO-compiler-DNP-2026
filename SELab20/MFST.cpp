#include "stdafx.h"
#include "MFST.h"
#include "Error.h"

#define MFST_DIAGN_MAXSIZE 2 * ERROR_MAXSIZE_MESSAGE

namespace MFST
{
    int FST_TRACE_n = -1;

    MfstState::MfstState() {
        lenta_position = 0;
        nrule = -1;
        nrulechain = -1;
    }

    MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain) {
        lenta_position = pposition;
        st = pst;
        nrule = pnrule;
        nrulechain = pnrulechain;
    }

    Mfst::MfstDiagnosis::MfstDiagnosis() {
        lenta_position = -1;
        rc_step = SURPRISE;
        nrule = -1;
        nrule_chain = -1;
    }

    Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain) {
        lenta_position = plenta_position;
        rc_step = prc_step;
        nrule = pnrule;
        nrule_chain = pnrule_chain;
    }

    Mfst::Mfst() {
        lenta = 0;
        lenta_size = lenta_position = 0;
        nrule = nrulechain = -1;
    }

    Mfst::Mfst(LT::LexTable plex, GRB::Greibach pgrebach) {
        grebach = pgrebach;
        lex = plex;
        lenta_size = lex.size;
        lenta = new GRBALPHABET[lenta_size + 1];
        for (int i = 0; i < lenta_size; i++)
            lenta[i] = GRB::Rule::Chain::T(lex.table[i].lexema[0]);
        lenta[lenta_size] = grebach.stbottomT;
        lenta_size++;
        lenta_position = 0;
        st.push(grebach.stbottomT);
        st.push(grebach.startN);
        nrule = -1;
        nrulechain = -1;
    }

    Mfst::~Mfst() {
        delete[] lenta;
    }

    bool Mfst::push_chain(GRB::Rule::Chain chain) {
        for (int k = chain.size - 1; k >= 0; k--)
            st.push(chain.nt[k]);
        return true;
    }

    bool Mfst::savestate() {
        storestate.push(MfstState(lenta_position, st, nrule, nrulechain));
        std::cout << std::setw(4) << std::left << FST_TRACE_n << ": "
            << std::setw(20) << std::left << "SAVESTATE:"
            << storestate.size() << std::endl;
        return true;
    }

    bool Mfst::reststate() {
        bool rc = false;
        MfstState state;
        if (rc = (storestate.size() > 0)) {
            state = storestate.top();
            lenta_position = state.lenta_position;
            st = state.st;
            nrule = state.nrule;
            nrulechain = state.nrulechain;
            storestate.pop();

            char bufLenta[256], bufSt[256];
            std::cout << std::setw(4) << std::left << FST_TRACE_n << ": "
                << std::setw(20) << std::left << "RESSTATE" << std::endl;
            std::cout << std::setw(4) << std::left << FST_TRACE_n << ": "
                << std::setw(20) << std::left << " "
                << std::setw(30) << std::left << getCLenta(bufLenta, lenta_position)
                << std::setw(20) << std::left << getCSt(bufSt) << std::endl;
        }
        return rc;
    }

    bool Mfst::savediagnosis(RC_STEP pprc_step) {
        bool rc = false;
        short k = 0;
        // Ищем место для сохранения самой глубокой позиции на ленте
        while (k < MFST_DIAGN_NUMBER && lenta_position <= diagnosis[k].lenta_position)
            k++;
        if (rc = (k < MFST_DIAGN_NUMBER)) {
            diagnosis[k] = MfstDiagnosis(lenta_position, pprc_step, nrule, nrulechain);
            for (short j = k + 1; j < MFST_DIAGN_NUMBER; j++)
                diagnosis[j].lenta_position = -1;
        }
        return rc;
    }

    char* Mfst::getDiagnosis(short n, char* buf) {
        char* rc = (char*)"";
        int errid = 0;
        int lpos = -1;
        if (n < MFST_DIAGN_NUMBER && (lpos = diagnosis[n].lenta_position) >= 0) {
            errid = grebach.getRule(diagnosis[n].nrule).iderror;
            Error::ERROR err = Error::geterror(errid);
            int line = (lpos < lex.size) ? lex.table[lpos].sn : lex.table[lex.size - 1].sn;
            sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: строка %d, %s", err.id, line, err.message);
            rc = buf;
        }
        return rc;
    }

    Mfst::RC_STEP Mfst::step() {
        RC_STEP rc = SURPRISE;
        char bufRule[256], bufLenta[256], bufSt[256];

        if (lenta_position < lenta_size) {
            if (st.empty()) return LENTA_END;

            if (GRB::Rule::Chain::isN(st.top())) {
                GRB::Rule rule;
                if ((nrule = grebach.getRule(st.top(), rule)) >= 0) {
                    GRB::Rule::Chain chain;
                    if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain)) >= 0) {
                        std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": "
                            << std::setw(20) << std::left << rule.getCRule(bufRule, nrulechain)
                            << std::setw(30) << std::left << getCLenta(bufLenta, lenta_position)
                            << std::setw(20) << std::left << getCSt(bufSt) << std::endl;
                        savestate();
                        st.pop();
                        push_chain(chain);
                        rc = NS_OK;
                        std::cout << std::setw(4) << std::left << FST_TRACE_n << ": "
                            << std::setw(20) << std::left << " "
                            << std::setw(30) << std::left << getCLenta(bufLenta, lenta_position)
                            << std::setw(20) << std::left << getCSt(bufSt) << std::endl;
                    }
                    else {
                        std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": "
                            << "TNS_NORULECHAIN/NS_NORULE" << std::endl;
                        savediagnosis(NS_NORULECHAIN);
                        rc = reststate() ? NS_NORULECHAIN : NS_NORULE;
                    }
                }
                else rc = NS_ERROR;
            }
            else if (st.top() == lenta[lenta_position]) {
                lenta_position++;
                st.pop();
                nrulechain = -1;
                rc = TS_OK;
                std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": "
                    << std::setw(20) << std::left << " "
                    << std::setw(30) << std::left << getCLenta(bufLenta, lenta_position)
                    << std::setw(20) << std::left << getCSt(bufSt) << std::endl;
            }
            else {
                std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": "
                    << "TS_NOK/NS_NORULECHAIN" << std::endl;
                rc = reststate() ? TS_NOK : NS_NORULE;
            }
        }
        else {
            rc = LENTA_END;
            std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": "
                << "LENTA_END" << std::endl;
        }
        return rc;
    }

    bool Mfst::start() {
        bool rc = false;
        RC_STEP rc_step = SURPRISE;
        char buf[MFST_DIAGN_MAXSIZE];
        FST_TRACE_n = -1;

        rc_step = step();
        while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK)
            rc_step = step();

        switch (rc_step) {
        case LENTA_END:
            std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " << "------>LENTA_END" << std::endl;
            std::cout << "-------------------------------------------------------------------------------------" << std::endl;
            sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: всего строк %d, синтаксический анализ выполнен без ошибок", 0, lex.table[lex.size - 1].sn);
            std::cout << buf << std::endl;
            rc = true;
            break;
        case NS_NORULE:
            std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " << "------>NS_NORULE" << std::endl;
            std::cout << "-------------------------------------------------------------------------------------" << std::endl;
            std::cout << getDiagnosis(0, buf) << std::endl;
            std::cout << getDiagnosis(1, buf) << std::endl;
            std::cout << getDiagnosis(2, buf) << std::endl;
            break;
        case NS_NORULECHAIN:
            std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " << "------>NS_NORULECHAIN" << std::endl;
            break;
        case NS_ERROR:
            std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " << "------>NS_ERROR" << std::endl;
            break;
        case SURPRISE:
            std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " << "------>SURPRISE" << std::endl;
            break;
        }
        return rc;
    }

    char* Mfst::getCSt(char* buf) {
        MFSTSTSTACK temp = st;
        int i = 0;
        while (!temp.empty()) {
            buf[i++] = GRB::Rule::Chain::alphabet_to_char(temp.top());
            temp.pop();
        }
        buf[i] = '\0';
        return buf;
    }

    char* Mfst::getCLenta(char* buf, short pos, short n) {
        int i = 0;
        for (; i < n && (pos + i) < lenta_size; i++)
            buf[i] = GRB::Rule::Chain::alphabet_to_char(lenta[pos + i]);
        buf[i] = '\0';
        return buf;
    }

    void Mfst::printrules() {
        MfstState state;
        GRB::Rule rule; 
        char buf[256];
        std::stack<MfstState> temp = storestate;
        std::stack<MfstState> rev;
        while (!temp.empty()) {
            rev.push(temp.top());
            temp.pop();
        }
        while (!rev.empty()) {
            state = rev.top();
            rev.pop();
            rule = grebach.getRule(state.nrule);
            std::cout << std::setw(4) << std::left << state.lenta_position << ": "
                << std::setw(20) << std::left << rule.getCRule(buf, state.nrulechain) << std::endl;
        }
    }
}