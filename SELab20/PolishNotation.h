#pragma once
#include "LT.h"
#include "IT.h"

// Построение обратной польской записи в таблице лексем на месте.
// Возвращает true при успешном преобразовании, false — при ошибке.
bool PolishNotation(
    int lextable_pos,        // позиция начала выражения в lextable
    LT::LexTable& lextable,  // таблица лексем
    IT::IdTable& idtable     // таблица идентификаторов
);

// Поиск и преобразование всех выражений в таблице лексем (после '=', 'r', 'p')
void ConvertAllExpressions(
    LT::LexTable& lextable,
    IT::IdTable& idtable
);