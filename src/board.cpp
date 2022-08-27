// ----------------------------------------------------------------------------
/* Copyright (c) 2018, Lucas Moesch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#include "board.hpp"

// Create an IODeviceWrapper around the Uart Peripheral we want to use
Board::LoggerDevice loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);