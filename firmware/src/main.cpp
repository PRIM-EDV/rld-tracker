/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#include "board/board.hpp"
#include "src/threads/lora.hpp"
#include "src/threads/gps.hpp"

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

    lora::thread.initialize();
    gps::thread.initialize();

    uint8_t c;
    while(true) {
        lora::thread.run();
        gps::thread.run(); 
    }
}