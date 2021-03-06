#include <cmath>
#include <iostream>
#include "single_frame_handler.h"
#include "frame.h"

single_frame_handler::single_frame_handler(int n1, int n2, int z1, int z2, int m) :
        D(1), N1(n1), N2(n2), m(m), Z1(z1), Z2(z2) {}

void single_frame_handler::move_blocks(queue_characteristics *from, queue_characteristics *to, int length) {
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
 *      <li> Выделение памяти под N1 свободных блоков;
 *      <li> Установление адресов связки в N1 свободных блоков;
 *              Адресные поля нулевого и последнего элементов выставляются отдельно.
 *              Предыдущий элемент для нулевого блока - NULL,
 *              следующий элемент для последнего блока - NULL.
 *      <li> Переход к DISP1.
 * </ol></ul>
 */
auto single_frame_handler::p1() {
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
 * т.е. сформировать пакет данных в этих свободных блоках. В результате в первых N2 свободных блоках
 * установлены пакеты данных, образуя очередь пакетов Оп32. Данные зависят от константы m.
 *
 *
 */
auto single_frame_handler::p2() {
    std::cout << "=== Switch to P2 ===" << std::endl;
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
auto single_frame_handler::p3() {
    std::cout << "=== Switch to P3 ===" << std::endl;
    p32 = new queue_characteristics();
    move_blocks(free, p32, N2);
    D++;
}

/*! \brief Программа P4. Формирование информационного кадра, включающего первый пакет в очереди пакетов Оп32.
 *
 * \details Установить исходные состояния: V(S) = Z1, N(S) = V(S).
 * Определить V(R) = Z2.
 * Записать биты заголовка <2-4> = N(S), биты <6-8> = V(R), бит <1> = 1 (тип кадра - информационный).
 * Сформировать КПК как результат сложения по модулю 2 заголовка кадра, заголовка пакета с байтом информационной части
 * пакета.
 */
auto single_frame_handler::p4() {
    std::cout << "=== Switch to P4 ===" << std::endl;
    VS = Z1;
    VR = Z2;
    NS = VS;
    VS++;

    auto frame = &(p32->first->_frame);
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

    D++;
}

/*! \brief Программа P5. Перенос информационного кадра, сформированного программой P4, в очередь повтора Оповт и
 * в регистр на передачу в канал.
 */
auto single_frame_handler::p5() {
    std::cout << "=== Switch to P5 ===" << std::endl;
    repeat = new queue_characteristics();
    move_blocks(p32, repeat, 1);
    mode = 1;
    output = &(repeat->first->_frame);

    std::cout << "Output register:" << std::endl;
    output->print();
    std::cout << "Repeat queue:" << std::endl;
    repeat->first->_frame.print();

    D++;
}

/*! \brief Программа P6. Формирование принятого кадра “RR”, подтверждающего правильный прием переданного
 * на противоположную сторону информационного кадра “I”.
 */
auto single_frame_handler::p6() {
    std::cout << "=== Switch to P6 ===" << std::endl;
    mode = 0;

    input = new input_frame();

    auto rr_ns = (output->frame_header >> 1) & 0x07;
    auto rr_nr = rr_ns + 1;

    input->frame_header = (rr_nr << 5) | 1;
    input->control[1] = input->frame_header;

    input->print();

    D++;
}

/*! \brief Программа P7. Запись этого кадра RR с контрольно-проверочной комбинацией КПК в первый блок
 * очереди Освоб.
 */
auto single_frame_handler::p7() {
    std::cout << "=== Switch to P7 ===" << std::endl;

    block *first = free->first;
    first->_frame.clear();
    first->_frame.frame_header = input->frame_header;

    D++;
}

/*! \brief Программа P8. Перенос принятого кадра RR из Освоб в очередь Окпм.
 */
auto single_frame_handler::p8() {
    std::cout << "=== Switch to P8 ===" << std::endl;
    cmp = new queue_characteristics();

    move_blocks(free, cmp, 1);

    free->print();
    cmp->print();

    D++;
}

/*! \brief Программа P9. Проверка правильного приема переданного ранее кадра “I” и находящегося в очереди повтора Оповт.
 */
auto single_frame_handler::p9() {
    std::cout << "=== Switch to P9 ===" << std::endl;
    unsigned char rr_ns = (repeat->first->_frame.frame_header >> 1) & 0x07;
    unsigned char rr_nr = (cmp->first->_frame.frame_header >> 5) & 0x07;

    std::cout << "NS = " << (int) rr_ns << std::endl;
    std::cout << "NR = " << (int) rr_nr << std::endl;

    if (rr_ns != rr_nr - 1) {
        throw std::exception();
    }
    D++;
}

/*! \brief Программа P10. Считывание кадров “RR” из очереди Окпм и “I” из Оповт и установка их в очередь Освоб.
 */
auto single_frame_handler::p10() {
    std::cout << "=== Switch to P10 ===" << std::endl;
    move_blocks(cmp, free, 1);
    free->last->_frame.clear();
    move_blocks(repeat, free, 1);

    repeat->clear();
    cmp->clear();

    free->print();
    std::cout << "Last but one element: " << std::endl;
    free->last->_previous->print();
    std::cout << "Last element: " << std::endl;
    free->last->print();

    D++;
}

/*! \brief Программа P11. Установление режима передачи очередного информационного кадра “I” в канал.
 */
auto single_frame_handler::p11() {
    std::cout << "=== Switch to P11 ===" << std::endl;
    mode = 1;

    D++;
}

/*! \brief Диспетчер 1. Программа формирования и передачи в канал связи одного информационного кадра
 *
 * \details Цель работы: составление и отладка программы формирования и передачи в канал связи
 * одного информационного кадра.
 */
void single_frame_handler::disp1() {
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

/*! \brief Диспетчер 2. Программа приема c канала кадра “RR”
 *
 * \details Цель работы: составление и отладка программы приема неискаженного информационного кадра “RR”
 * в ответ на передачу кадра “I” при выполнении лабораторной работы №1.
 */
void single_frame_handler::disp2() {
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
            case 10:
                p10();
                break;
            case 11:
                p11();
                break;
            default:
                need_break = true;
                break;
        }
    }
    std::cout << "Exiting..." << std::endl;
}