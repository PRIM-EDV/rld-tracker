/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#include "board/board.hpp"
#include "src/threads/lora.hpp"

#include "driver/gps/protocol/nmea0183.hpp"

using namespace Board;
namespace Board::lora{
    LoraThread<Spi, Nss, D0> thread;
}

int main()
{
    Board::initialize();

    NMEA0183<Board::gps::Uart> nmea;
    // nmea.message = nullptr;

    lora::thread.initialize();

    while(true) {
        lora::thread.run();
        
    }
}