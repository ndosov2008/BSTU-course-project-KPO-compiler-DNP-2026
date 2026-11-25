#include "stdafx.h"
#include "Error.h"

namespace Error
{
    ERROR errors[ERROR_MAX_ENTRY] =
    {
        ERROR_ENTRY(0, "Недопустимый код ошибки"),
        ERROR_ENTRY(1, "Системный сбой"),
        ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5),
        ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
        ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40), ERROR_ENTRY_NODEF10(50),
        ERROR_ENTRY_NODEF10(60), ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), ERROR_ENTRY_NODEF10(90),

        ERROR_ENTRY(100, "Параметр -in должен быть задан"),
        ERROR_ENTRY_NODEF(101), ERROR_ENTRY_NODEF(102), ERROR_ENTRY_NODEF(103),
        ERROR_ENTRY(104, "Превышена длина входного параметра"),
        ERROR_ENTRY_NODEF(105), ERROR_ENTRY_NODEF(106), ERROR_ENTRY_NODEF(107), ERROR_ENTRY_NODEF(108), ERROR_ENTRY_NODEF(109),
        ERROR_ENTRY(110, "Ошибка при открытии файла с исходным кодом (-in)"),
        ERROR_ENTRY(111, "Недопустимый символ в исходном файле (-in)"),
        ERROR_ENTRY(112, "Ошибка при создании файла протокола (-log)"),
        ERROR_ENTRY(113, "Ошибка при создании выходного файла (-out)"),
        ERROR_ENTRY_NODEF(114), ERROR_ENTRY_NODEF(115), ERROR_ENTRY_NODEF(116), ERROR_ENTRY_NODEF(117), ERROR_ENTRY_NODEF(118), ERROR_ENTRY_NODEF(119),
        ERROR_ENTRY_NODEF10(120), ERROR_ENTRY_NODEF10(130), ERROR_ENTRY_NODEF10(140), ERROR_ENTRY_NODEF10(150),
        ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),

        ERROR_ENTRY(200, "Неизвестная лексема"),
        ERROR_ENTRY(201, "Переполнение таблицы лексем"),
        ERROR_ENTRY(202, "Переполнение таблицы идентификаторов"),
        ERROR_ENTRY(203, "Превышена максимальная длина идентификатора (16 символов)"),
        ERROR_ENTRY(204, "Некорректный литерал (переполнение или неверный формат)"),
        ERROR_ENTRY_NODEF(205), ERROR_ENTRY_NODEF(206), ERROR_ENTRY_NODEF(207), ERROR_ENTRY_NODEF(208), ERROR_ENTRY_NODEF(209),
        ERROR_ENTRY_NODEF10(210), ERROR_ENTRY_NODEF10(220), ERROR_ENTRY_NODEF10(230), ERROR_ENTRY_NODEF10(240), ERROR_ENTRY_NODEF10(250),
        ERROR_ENTRY_NODEF10(260), ERROR_ENTRY_NODEF10(270), ERROR_ENTRY_NODEF10(280), ERROR_ENTRY_NODEF10(290),

        ERROR_ENTRY(300, "Использование необъявленного идентификатора"),
        ERROR_ENTRY(301, "Отсутствует точка входа main"),
        ERROR_ENTRY(302, "Обнаружено несколько точек входа main"),
        ERROR_ENTRY(303, "Некорректное объявление функции или переменной"),
        ERROR_ENTRY(304, "В объявлении отсутствует ключевое слово"),
        ERROR_ENTRY(305, "Попытка переопределения идентификатора"),
        ERROR_ENTRY(306, "Превышено максимальное количество параметров функции"),
        ERROR_ENTRY(307, "Слишком много аргументов в вызове функции"),
        ERROR_ENTRY(308, "Количество параметров функции и переданных аргументов не совпадает"),
        ERROR_ENTRY(309, "Недопустимый пустой строковый литерал"),
        ERROR_ENTRY(310, "Несовпадение типов передаваемых аргументов функции"),
        ERROR_ENTRY(311, "Не закрыт строковый или символьный литерал"),
        ERROR_ENTRY(312, "Неверное число аргументов стандартной функции"),
        ERROR_ENTRY(313, "Недопустимый литерал: ошибка преобразования значения"),
        ERROR_ENTRY(314, "Несоответствие типов данных в выражении или присваивании"),
        ERROR_ENTRY(315, "Несоответствие типа функции и возвращаемого значения"),
        ERROR_ENTRY(316, "Недопустимое использование арифметических операций над строками или символами"),
        ERROR_ENTRY(317, "Недопустимое условное выражение: сравнение возможно только для типа short"),
        ERROR_ENTRY(318, "Деление на ноль"),
        ERROR_ENTRY(319, "Превышен допустимый размер строкового литерала (255 символов)"),
        ERROR_ENTRY_NODEF10(320), ERROR_ENTRY_NODEF10(330), ERROR_ENTRY_NODEF10(340), ERROR_ENTRY_NODEF10(350),
        ERROR_ENTRY_NODEF10(360), ERROR_ENTRY_NODEF10(370), ERROR_ENTRY_NODEF10(380), ERROR_ENTRY_NODEF10(390),

        ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),

        ERROR_ENTRY(600, "Неверная структура программы"),
        ERROR_ENTRY(601, "Ошибочный оператор"),
        ERROR_ENTRY(602, "Ошибка в выражении"),
        ERROR_ENTRY(603, "Ошибка в параметрах функции"),
        ERROR_ENTRY(604, "Ошибка в аргументах вызываемой функции"),
        ERROR_ENTRY(605, "Ошибка в условном выражении"),
        ERROR_ENTRY_NODEF(606), ERROR_ENTRY_NODEF(607), ERROR_ENTRY_NODEF(608), ERROR_ENTRY_NODEF(609),
        ERROR_ENTRY_NODEF10(610), ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
        ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680),
        ERROR_ENTRY_NODEF10(690), ERROR_ENTRY_NODEF100(700), ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900)
    };

    ERROR geterror(int id)
    {
        if (id <= 0 || id >= ERROR_MAX_ENTRY) id = 0;
        for (size_t i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i)
        {
            if (errors[i].id == id)
            {
                ERROR err = errors[i];
                err.inext.line = -1;
                err.inext.col = -1;
                return err;
            }
        }
        return errors[0];
    }

    ERROR geterrorin(int id, int line, int col)
    {
        ERROR err = geterror(id);
        err.inext.line = (short)line;
        err.inext.col = (short)col;
        return err;
    }
}