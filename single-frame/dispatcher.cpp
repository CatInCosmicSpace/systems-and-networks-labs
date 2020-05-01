#include "easylogging++.h"
#include "dispatcher.h"
#include <bitset>

INITIALIZE_EASYLOGGINGPP

dispatcher::dispatcher(int n1, int n2, int m, int z1, int z2):
        D(1), N1(n1), N2(n2), m(m), Z1(z1), Z2(z2) {}

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

    NS = VS;
    VS++;

    p32.front()->frame_header = (NS << 1) | (VR << 5);
    p32.front()->control = p32.front()->frame_header ^ p32.front()->data[0];

    D++;
}

/*! \brief Программа P5. Перенос информационного кадра, сформированного программой P4, в очередь повтора Оповт и
 * в регистр на передачу в канал.
 */
auto dispatcher::p5() {
    LOG(INFO) << "=== Switch to P5 ===";
    mode = 1;
    repeat.push(p32.front());
    output.push(p32.front());
    p32.pop();

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

    auto rr_ns = (output.front()->frame_header >> 1) & 0x07;
    auto rr_nr = rr_ns + 1;
    NR = ((repeat.front()->frame_header >> 1) & 0x03) + 1;//NS + 1;
    rr.frame_header = (rr_nr << 5) | 1;
    rr.control = rr.frame_header << 8;
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

    D++;
}

/*! \brief Программа P9. Проверка правильного приема переданного ранее кадра “I” и находящегося в очереди повтора Оповт.
 */
auto dispatcher::p9() {
    LOG(INFO) << "=== Switch to P9 ===";
    unsigned char rr_ns = (repeat.front()->frame_header >> 1) & 0x07;
    unsigned char rr_nr = (cmp.front()->frame_header >> 5) & 0x07;

    LOG(INFO) << "NS = " << (int)rr_ns;
    LOG(INFO) << "NR = " << (int)rr_nr;

    if (rr_ns == rr_nr - 1) {
        LOG(INFO) << "NR equals to NS + 1";
    } else {
        throw std::exception();
    }
    D++;
}

/*! \brief Программа P10. Считывание кадров “RR” из очереди Окпм и “I” из Оповт и установка их в очередь Освоб.
 */
auto dispatcher::p10() {
    LOG(INFO) << "=== Switch to P10 ===";
    free.push(cmp.front());
    cmp.pop();
    std::fill_n(free.back()->data, 128, 0);
    free.push(repeat.front());
    repeat.pop();

    D++;
}

/*! \brief Программа P11. Установление режима передачи очередного информационного кадра “I” в канал.
 */
auto dispatcher::p11() {
    LOG(INFO) << "=== Switch to P11 ===";
    mode = 1;
    D++;
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
            case 10: p10(); break;
            case 11: p11(); break;
            default: need_break = true; break;
        }
    }
    LOG(INFO) << "Exiting...";
}