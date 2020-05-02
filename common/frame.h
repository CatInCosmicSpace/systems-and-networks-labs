#ifndef LABS_FRAME_H
#define LABS_FRAME_H

#include <cstdint>

struct frame {
    uint8_t  packet_header[3];  /*!< Заголовок пакета (3 байта) */
    uint8_t  frame_header;      /*!< Заголовок кадра (1 байт) */
    uint8_t  data[128];         /*!< Информационная часть пакета данных (128 байт) */
    uint8_t  control[2];           /*!< Контрольно-проверочная комбинация (КПК) (2 байта) */

    frame();

    void clear();
    void print();
};


#endif //LABS_FRAME_H
