#ifndef LABS_DISPATCHER_H
#define LABS_DISPATCHER_H

#include "queue_characteristics.h"
#include "receiver_ready.h"
#include "block.h"

/*! \brief Диспетчер 1.
 *
 * \details
 * <ul>
 *      <li> Константы N1, N2, Z1, Z2, m;
 *      <li> Переменные D, V(S), V(R), N(S), N(R), n;
 *      <li> Программы DISP1, P1, P2, P3, P4, P5;
 *      <li> Очереди Оп32, Оповт, Освоб;
 *      <li> Регистр передачи в канал RGвых.
 * </ul>
 *
 * \author Егорова Алена, группа ИУ8-104
 * \date 2020 год
*/
class dispatcher {
public:
    dispatcher(int N1, int N2, int Z1, int Z2, int Z3, int m, int cycles);

    void disp1();

    void disp2();

    void disp3();

private:
    size_t N1, N2, Z1, Z2, Z3, m, cycles, K = 1, CNR;
    size_t D;
    size_t VS; /*!< Переменная передачи состояния */
    size_t VR; /*!< Переменная состояния приёма */
    size_t NS; /*!< Порядковый номер передаваемого кадра I. N(S) устанавливается равным V(S) */
    size_t NR; /*!< Порядковый номер I-кадра, ожидаемого на приём */

    queue_characteristics *free;    /*!< Очередь свободных блоков */
    queue_characteristics *p32;     /*!< Очередь пакетов на передачу с сетевого уровня на канальный уровень */
    queue_characteristics *repeat;  /*!< Очередь информационных кадров на случай необходимости повторной передачи кадров в канал */
    queue_characteristics *cmp;

    frame *output;  /*!< Регистр выходных пакетов */
    receiver_ready *input;

    block *tadr = nullptr;

    uint8_t mode;

    auto p1();

    auto p2();

    auto p3();

    auto p4();

    auto p5();

    auto p6();

    auto p7();

    auto p8();

    auto p9();

    auto p10();

    auto p11();

    auto p12();

    void move_blocks(queue_characteristics *from, queue_characteristics *to, int length);
};


#endif //LABS_DISPATCHER_H
