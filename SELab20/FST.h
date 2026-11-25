#pragma once
#ifndef FST_H
#define FST_H

#include <iostream>

namespace FST
{
    struct RELATION // ребро: символ -> вершина графа переходов КА
    {
        char symbol;    // символ перехода
        short nnode;    // номер смежной вершины

        RELATION(
            char c = 0x00,  // символ перехода
            short ns = 0    // новое состояние (заменено NULL на 0 для совместимости типов)
        );
    };

    struct NODE // вершина графа переходов
    {
        short n_relation;   // количество инцидентных ребер
        RELATION* relations; // инцидентные ребра

        NODE();
        NODE(
            short n,        // количество инцидентных ребер
            RELATION rel, ... // список ребер
        );
    };

    struct FST // недетерминированный конечный автомат
    {
        char* string;   // цепочка (строка, завершается 0x00)
        short position; // текущая позиция в цепочке
        short nstates;  // количество состояний автомата
        NODE* nodes;    // граф переходов: [0]-начальное состояние, [nstate-1]-конечное
        short* rstates; // возможные состояния автомата на данной позиции

        FST(
            char* s,        // цепочка
            short ns,       // количество состояний автомата
            NODE n, ...     // список состояний (граф переходов)
        );
    };

    bool execute( // выполнить распознавание цепочки
        FST& fst  // недетерминированный конечный автомат
    );
};

#endif