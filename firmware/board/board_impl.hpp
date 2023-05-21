// ----------------------------------------------------------------------------
/* Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

namespace Board
{

namespace usb
{
    modm::IODeviceWrapper<Uart, modm::IOBuffer::DiscardIfFull> usbIODevice;
	modm::IOStream ioStream(usbIODevice);
}

}