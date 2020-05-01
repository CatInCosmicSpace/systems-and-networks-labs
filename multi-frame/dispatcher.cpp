#include "easylogging++.h"
#include "dispatcher.h"
#include <bitset>

INITIALIZE_EASYLOGGINGPP

dispatcher::dispatcher(int n1, int n2, int z1, int z2, int z3, int m, int cycles):
        D(1), N1(n1), N2(n2), m(m), Z1(z1), Z2(z2), Z3(z3), cycles(cycles) {}

/*! \brief Программа P1. Формирование очереди из N1 свободных блоков.
 *
 * \details
 * <ul><ol>
 *      <li> Выделение памяти под N1 свободных блоков.
 *      <li> Установление адресов связки в N1 свободных блоков.
 * </ol></ul>
 */
auto dispatcher::p1() {
    LOG(INFO) << "=== Switch to P1 ===";
    for (int i = 0; i < N1; ++i) {
        blocks.emplace_back();
    }
    for (int i = 1; i < N1 - 1; ++i) {
        blocks[i].previous = &blocks[i - 1];
        blocks[i].next = &blocks[i + 1];
    }

    blocks[0].previous = nullptr;
    blocks[0].next = &blocks[1];
    blocks[N1 - 1].previous = &blocks[N1 - 2];
    blocks[N1 - 1].next = nullptr;

    for (int i = 0; i < N1; ++i) {
        free.push(&blocks[i]);
    }

    D++;
}

/*! \brief Программа P2. Формирование N2 пакетов данных.
 *
 * \details Записать данные в информационную часть первых N2 свободных блоков очереди Освоб,
 * т.е. сформировать пакет данных в этих свободных блоках.
 */
auto dispatcher::p2() {
    LOG(INFO) << "=== Switch to P2 ===";
    int n = m;
    for (int i = 0; i < N2; ++i) {
        ++n;
        for (auto& data_byte: blocks[i].data) {
            data_byte = n;
        }
    }

    D++;
}

/*! \brief Программа P3. Перенос N2 пакетов данных из очереди Oсвоб в очередь Оп32.
 */
auto dispatcher::p3() {
    LOG(INFO) << "=== Switch to P3 ===";
    for (int i = 0; i < N2; ++i) {
        p32.push(free.front());
        free.pop();
    }

    D++;
}

/*! \brief Программа P4. Формирование информационного кадра, включающего первый пакет в очереди пакетов Оп32.
 */
auto dispatcher::p4() {
    LOG(INFO) << "=== Switch to P4 ===";
    VS = Z1;
    VR = Z2;

    block* current = p32.front();

    for (int i = 0; i < cycles; ++i) {
        NS = VS;
        ++VS;

        current->frame_header = (0x0E) & (NS << 1) | (0xE0) & (VR << 5);
        current->frame_header = (NS << 1) | (VR << 5);
        current->control = current->frame_header ^ current->data[0];

        current = current->next;
    }

    D++;
}

/*! \brief Программа P5. Перенос информационного кадра, сформированного программой P4, в очередь повтора Оповт и
 * в регистр на передачу в канал.
 */
auto dispatcher::p5() {
    LOG(INFO) << "=== Switch to P5 ===";
    mode = 1;
    for (int i = 0; i < cycles; ++i) {
        repeat.push(p32.front());
        p32.pop();
    }

    output.push(repeat.front());

    LOG(INFO) << "Output register:";

    LOG(INFO) << "Information frame parameters: ";
    LOG(INFO) << "Previous address: " << output.front()->previous;
    LOG(INFO) << "Next address:     " << output.front()->next;
    LOG(INFO) << "Frame header:     " << std::bitset<8>(output.front()->frame_header);
    LOG(INFO) << "Packet header:    ";
    for (int i = 0; i < 3; ++i) LOG(INFO) << "Packet header[" << i << "] = " << std::bitset<8>(output.front()->packet_header[i]);
    LOG(INFO) << "Data:             " << std::bitset<8>(output.front()->data[0]);
    LOG(INFO) << "Control:          " << std::bitset<16>(output.front()->control);
    LOG(INFO) << "N(S) = " << std::bitset<8>(NS) << "; V(R) = " << std::bitset<8>(VR) << "; Frame type: " << 0;

    D++;
}

/*! \brief Диспетчер 1. Программа формирования и передачи в канал связи одного информационного кадра
 */
void dispatcher::disp1() {
    LOG(INFO) << "=== Switch to Disp1 ===";
    auto need_break = false;
    while (!need_break) {
        switch (D) {
            case 1: p1(); break;
            case 2: p2(); break;
            case 3: p3(); break;
            case 4: p4(); break;
            case 5: p5(); break;
            default: need_break = true; break;
        }
    }
    LOG(INFO) << "Exiting...";
}

/*! \brief Программа P6. Формирование принятого кадра “RR”, подтверждающего правильный прием переданного
 * на противоположную сторону информационного кадра “I”.
 */
auto dispatcher::p6() {
    LOG(INFO) << "=== Switch to P6 ===";
    mode = 0;

    uint8_t reject_type = 0x05;
    rr.frame_header = (Z3 << 5) | reject_type;

    rr.control = rr.frame_header << 8;

    //Вывод принятого кадра PR
    LOG(INFO) << "Received REJ frame:";
    LOG(INFO) << "Receiver ready frame:";
    LOG(INFO) << "Header:           " << std::bitset<8>(rr.frame_header);
    LOG(INFO) << "Control:          " << std::bitset<16>(rr.control);

    D++;
}

/*! \brief Программа P7. Запись этого кадра RR с контрольно-проверочной комбинацией КПК в первый блок
 * очереди Освоб.
 */
auto dispatcher::p7() {
    LOG(INFO) << "=== Switch to P7 ===";
    std::fill_n(free.front()->data, 128, 0);
    free.front()->frame_header = rr.frame_header;

    D++;
}

/*! \brief Программа P8. Перенос принятого кадра RR из Освоб в очередь Окпм.
 */
auto dispatcher::p8() {
    LOG(INFO) << "=== Switch to P8 ===";
    cmp.push(free.front());
    free.pop();
    mode = 2;
    //Установка CNR соответственно NR из REJ
    CNR = (rr.frame_header >> 5) & 0x07;

    D++;
}

/*! \brief Программа P9. Проверка правильного приема переданного ранее кадра “I” и находящегося в очереди повтора Оповт.
 */
auto dispatcher::p9() {
    LOG(INFO) << "=== Switch to P9 ===";
    free.push(cmp.front());
    cmp.pop();

    D++;
}

/*! \brief Программа P10. Считывание кадров “RR” из очереди Окпм и “I” из Оповт и установка их в очередь Освоб.
 */
auto dispatcher::p10() {
    LOG(INFO) << "=== Switch to P10 ===";
    if (((tadr->frame_header >> 1) & 0x07) < CNR) {
        D = 11;
    } else {
        D = 12;
    }
}

/*! \brief Программа P11. Установление режима передачи очередного информационного кадра “I” в канал.
 */
auto dispatcher::p11() {
    LOG(INFO) << "=== Switch to P11 ===";
    free.push(repeat.front());
    repeat.pop();

    std::fill_n(free.back()->data, 128, 0);
    free.back()->control = 0;

    tadr = repeat.front();
    ++K;

    D = 10;
}

auto dispatcher::p12() {
    LOG(INFO) << "=== Switch to P12 ===";
    output.pop();
    output.push(tadr);

    if (K == cycles) {
        D++;
    } else {
        tadr = tadr->next;
        ++K;
        D = 10;
    }
}

/*! \brief Диспетчер 2. Программа приема c канала кадра “RR”
 */
void dispatcher::disp2() {
    LOG(INFO) << "=== Switch to Disp2 ===";
    auto need_break = false;
    while (!need_break) {
        switch (D) {
            case 6: p6(); break;
            case 7: p7(); break;
            case 8: p8(); break;
            case 9: p9(); break;
            default: need_break = true; break;
        }
    }
    LOG(INFO) << "Exiting...";
}



/*! \brief Диспетчер 3. Программа приема c канала кадра “RR”
 */
void dispatcher::disp3() {
    LOG(INFO) << "=== Switch to Disp3 ===";

    tadr = free.front();

    auto need_break = false;
    while (!need_break) {
        switch (D) {
            case 10: p10(); break;
            case 11: p11(); break;
            case 12: p12(); break;
            default: need_break = true; break;
        }
    }
    LOG(INFO) << "Exiting...";
}