#ifndef LABS12_BLOCK_H
#define LABS12_BLOCK_H

#include <bitset>
#include <vector>
#include <array>

/*! \brief Блоки данных.
 *  \author Егорова Алена, группа ИУ8-104
 *  \date 2020 год
*/
struct block {
    block*   previous;          /*!< Адрес предыдущего блока (2 байта) */
    block*   next;              /*!< Адрес следующего блока (2 байта) */
    uint8_t  packet_header[3];  /*!< Заголовок пакета (3 байта) */
    uint8_t  frame_header;      /*!< Заголовок кадра (1 байт). Бит 0 => тип кадра, биты 1-3 => N(S), биты 5-7 => V(R) */
    uint8_t  data[128];         /*!< Информационная часть пакета данных (128 байт) */
    uint16_t control;           /*!< Контрольно-проверочная комбинация (КПК) (2 байта) */
};

#endif //LABS12_BLOCK_H
