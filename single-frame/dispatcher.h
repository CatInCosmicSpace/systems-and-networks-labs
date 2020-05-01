#ifndef LABS12_DISPATCHER_H
#define LABS12_DISPATCHER_H

#include <queue>
#include "block.h"
#include "receiver_ready.h"

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
    dispatcher(int N1, int N2, int Z1, int Z2, int m);
    void disp1();
    void disp2();
private:
    size_t N1, N2, Z1, Z2, m;
    size_t D;
    size_t VS; /*!< Переменная передачи состояния */
    size_t VR; /*!< Переменная состояния приёма */
    size_t NS; /*!< Порядковый номер передаваемого кадра I. N(S) устанавливается равным V(S) */
    size_t NR; /*!< Порядковый номер I-кадра, ожидаемого на приём */

    std::queue<block*> free;    /*!< Очередь свободных блоков */

    std::queue<block*> p32;     /*!< Очередь пакетов на передачу с сетевого уровня на канальный уровень */
    std::queue<block*> p23;     /*!< Очередь пакетов на передачу с канального уровня на сетевой уровень */

    std::queue<block*> repeat;  /*!< Очередь информационных кадров на случай необходимости повторной передачи кадров в канал */

    std::queue<block*> output;  /*!< Регистр выходных пакетов */
    std::queue<block*> cmp;     /*!< Очередь всех принятых кадров с канала */

    std::vector<block> blocks;

    receiver_ready rr;

    bool mode;

    // LAB1
    auto p1();
    auto p2();
    auto p3();
    auto p4();
    auto p5();

    // LAB2
    auto p6();
    auto p7();
    auto p8();
    auto p9();
    auto p10();
    auto p11();
};


#endif //LABS12_DISPATCHER_H
