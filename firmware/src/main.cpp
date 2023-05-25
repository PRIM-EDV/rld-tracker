/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#include "board/board.hpp"
#include "src/threads/lora.hpp"
#include "src/threads/gps.hpp"
#include "shared/shared.hpp"

using namespace Board;

namespace Board::lora{
    LoraThread<Spi, Nss, D0> thread;
}

namespace Board::gps {
    GPSThread<Uart> thread;
}

int main()
{
    Board::initialize();

    shared::trackerId = 1;

    delay(1000ms); // Wait for BT-device
    bluetooth::ioStream << "AT+NAMERLD-" << shared::trackerId << "\r\n";

    lora::thread.initialize();
    gps::thread.initialize();

    fiber::Scheduler::run();
}