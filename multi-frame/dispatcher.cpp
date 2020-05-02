#include <bitset>
#include <cmath>
#include <iostream>
#include "dispatcher.h"
#include "frame.h"


dispatcher::dispatcher(int n1, int n2, int z1, int z2, int z3, int m, int cycles) :
        D(1), N1(n1), N2(n2), m(m), Z1(z1), Z2(z2), Z3(z3), cycles(cycles) {}

void dispatcher::move_blocks(queue_characteristics *from, queue_characteristics *to, int length) {
    if (!from || !to || length <= 0 || from->length == 0)
        return;

    int to_move = std::fmin(from->length, length);

    //Сохраняем указатель на первый блок, переносимый в новую очередь
    block *first_to_move = from->first;

    block *last_to_move = from->first;
    for (int i = 0; i < to_move - 1; ++i) {
        last_to_move = last_to_move->_next;
    }

    from->length -= to_move;
    from->first = last_to_move->_next;

    if (from->length != 0) {
        from->first->_previous = nullptr;
    } else {
        from->last = nullptr;
    }

    last_to_move->_next = nullptr;

    if (to->length == 0) {
        to->first = first_to_move;
    } else {
        to->last->_next = first_to_move;
        first_to_move->_previous = to->last;
    }

    to->last = last_to_move;
    to->length += to_move;
}


/*! \brief Программа P1. Формирование очереди из N1 свободных блоков.
 *
 * \details
 * <ul><ol>
 *      <li> Выделение памяти под N1 свободных блоков.
 *      <li> Установление адресов связки в N1 свободных блоков.
 * </ol></ul>
 */
auto dispatcher::p1() {
    std::cout << "=== Switch to P1 ===" << std::endl;
    block *blocks = new block[N1];

    free = new queue_characteristics(&blocks[0], &blocks[N1 - 1], N1);

    blocks[0]._previous = nullptr;
    blocks[0]._next = &blocks[1];
    blocks[N1 - 1]._previous = &blocks[N1 - 2];
    blocks[N1 - 1]._next = nullptr;

    for (int i = 1; i < N1 - 1; ++i) {
        blocks[i]._previous = &blocks[i - 1];
        blocks[i]._next = &blocks[i + 1];
    }

    D++;
}

/*! \brief Программа P2. Формирование N2 пакетов данных.
 *
 * \details Записать данные в информационную часть первых N2 свободных блоков очереди Освоб,
 * т.е. сформировать пакет данных в этих свободных блоках.
 */
auto dispatcher::p2() {
    std::cout << "=== Switch to P3 ===" << std::endl;
    block *current = free->first;
    int n = m;
    for (int i = 0; i < N2; ++i) {
        ++n;
        std::fill_n(current->_frame.data, 128, n);
        current = current->_next;
    }

    D++;
}

/*! \brief Программа P3. Перенос N2 пакетов данных из очереди Oсвоб в очередь Оп32.
 */
auto dispatcher::p3() {
    std::cout << "=== Switch to P3 ===" << std::endl;
    p32 = new queue_characteristics();
    move_blocks(free, p32, N2);

    D++;
}

/*! \brief Программа P4. Формирование массивов MСICL информационных кадров с входящими в него пакетами
 * в очереди пакетов Оп32 пакетов данных.
 */
auto dispatcher::p4() {
    std::cout << "=== Switch to P4 ===" << std::endl;
    VS = Z1;
    VR = Z2;

    block *current = p32->first;

    for (int i = 0; i < cycles; ++i) {
        NS = VS;
        ++VS;

        auto frame = &(current->_frame);
        frame->frame_header = (0x0E) & (NS << 1) | (0xE0) & (VR << 5);

        uint8_t high = 0;
        for (int i = 0; i < 128; i += 1) {
            high ^= frame->data[i];
        }

        uint8_t low = frame->frame_header;

        for (int i = 0; i < 3; i += 1) {
            low ^= frame->packet_header[i];
        }

        low ^= frame->data[0];

        //Запись контрольной суммы
        frame->control[0] = high;
        frame->control[1] = low;

        current = current->_next;
    }

    D++;
}

/*! \brief Программа P5. Перенос информационных кадров MСICL, сформированных программой P4, в очередь повтора Оповт и
 * в регистр на передачу в канал..
 */
auto dispatcher::p5() {
    std::cout << "=== Switch to P5 ===" << std::endl;

    repeat = new queue_characteristics();
    move_blocks(p32, repeat, cycles);
    mode = 1;
    output = &(repeat->first->_frame);

    repeat->print();
    output->print();

    block *current = repeat->first;
    for (int i = 1; current != nullptr; ++i) {
        std::cout << "Frame #" << i << std::endl;
        current->_frame.print();
        current = current->_next;
    }

    D++;
}

/*! \brief Диспетчер 1. Программа формирования и передачи в канал связи одного информационного кадра
 */
void dispatcher::disp1() {
    std::cout << "=== Switch to DISP1 ===" << std::endl;
    auto need_break = false;
    while (!need_break) {
        switch (D) {
            case 1:
                p1();
                break;
            case 2:
                p2();
                break;
            case 3:
                p3();
                break;
            case 4:
                p4();
                break;
            case 5:
                p5();
                break;
            default:
                need_break = true;
                break;
        }
    }
    std::cout << "Exiting..." << std::endl;
}

/*! \brief Программа P6. Фформирование принятого кадра “REJ”, требующего передачу
 * на противоположную сторону кадров “I” с очереди повтора Оповт.
 */
auto dispatcher::p6() {
    std::cout << "=== Switch to P6 ===" << std::endl;
    mode = 0;

    input = new receiver_ready();

    uint8_t reject_type = 0x05;
    input->frame_header = (Z3 << 5) | reject_type;
    input->control[0] = input->frame_header;

    input->print();

    D++;
}

/*! \brief запись этого кадра “REJ”, с контрольно-проверочной комбинацией КПК в первый массив блока очереди Освоб.
 * Проверка безошибочного приема кадра REJ с канала связи.
 */
auto dispatcher::p7() {
    std::cout << "=== Switch to P7 ===" << std::endl;

    block *first = free->first;
    first->_frame.clear();
    first->_frame.frame_header = input->frame_header;

    D++;
}

/*! \brief перенос кадра REJ (без КПК), поступившего неискаженным, из Освоб в очередь принятых с канала кадров Окпм.
 * Установление режима передачи в канал информационного кадра “I” c очереди повтора Оповт.
 */
auto dispatcher::p8() {
    std::cout << "=== Switch to P8 ===" << std::endl;
    cmp = new queue_characteristics();
    move_blocks(free, cmp, 1);
    mode = 2;
    CNR = (input->frame_header >> 5) & 0x07;

    free->print();
    cmp->print();
    D++;
}

/*! \brief Программа P9. поставить кадр REJ с начальным адресом массива из очереди принятых с канала кадров
 * Окпм А(N2+1) в конец очереди свободных блоков Освоб.
 */
auto dispatcher::p9() {
    std::cout << "=== Switch to P9 ===" << std::endl;
    move_blocks(cmp, free, 1);
    cmp->clear();

    D++;
}

/*! \brief Программа P10. Программа проверки необходимости стирания и повторной передачи кадров “I” с очереди повтора Оповт.
 */
auto dispatcher::p10() {
    std::cout << "=== Switch to P10 ===" << std::endl;
    if (((tadr->_frame.frame_header >> 1) & 0x07) < CNR) {
        D = 11;
    } else {
        D = 12;
    }
}

/*! \brief Программа P11. программа стирания кадра (кадров) “I” с очереди повтора Оповт.
 */
auto dispatcher::p11() {
    std::cout << "=== Switch to P11 ===" << std::endl;

    move_blocks(repeat, free, 1);

    std::fill_n(free->last->_frame.data, 128, 0);
    std::fill_n(free->last->_frame.control, 2, 0);

    tadr = repeat->first;
    ++K;

    D = 10;
}

/*! \brief Программа P12. программа передачи в канал кадров “I” с очереди повтора Оповт.
 */
auto dispatcher::p12() {
    std::cout << "=== Switch to P12 ===" << std::endl;
    output = &(tadr->_frame);

    if (K == cycles) {
        output->print();
        D++;
    } else {
        tadr = tadr->_next;
        ++K;
        D = 10;
    }
}

/*! \brief Диспетчер 2. Программа приема c канала кадра “RR”
 */
void dispatcher::disp2() {
    std::cout << "=== Switch to DISP2 ===" << std::endl;
    auto need_break = false;
    while (!need_break) {
        switch (D) {
            case 6:
                p6();
                break;
            case 7:
                p7();
                break;
            case 8:
                p8();
                break;
            case 9:
                p9();
                break;
            default:
                need_break = true;
                break;
        }
    }
    std::cout << "Exiting..." << std::endl;
}


/*! \brief Диспетчер 3. Программа приема c канала кадра “RR”
 */
void dispatcher::disp3() {
    std::cout << "=== Switch to DISP3 ===" << std::endl;
    tadr = free->first;
    auto need_break = false;
    while (!need_break) {
        switch (D) {
            case 10:
                p10();
                break;
            case 11:
                p11();
                break;
            case 12:
                p12();
                break;
            default:
                need_break = true;
                break;
        }
    }
    std::cout << "Exiting..." << std::endl;
}