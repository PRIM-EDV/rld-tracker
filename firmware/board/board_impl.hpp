// ----------------------------------------------------------------------------
/* Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

namespace Board
{
namespace bluetooth
{
    modm::IODeviceWrapper<Uart, modm::IOBuffer::BlockIfFull> device;
	modm::IOStream ioStream(device);
}
namespace usb
{
    modm::IODeviceWrapper<Uart, modm::IOBuffer::BlockIfFull> device;
	modm::IOStream ioStream(device);
}
}