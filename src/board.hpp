// ----------------------------------------------------------------------------
/* Copyright (c) 2018, Lucas Moesch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#define XPCC_BOARD_HAS_LOGGER

using namespace xpcc::stm32;

namespace Board
{

/// STM32F0 running at 48MHz generated from the internal 8MHz with PLL.
struct systemClock
{
	// core and bus frequencys
	static constexpr uint32_t Frequency = MHz48;
	static constexpr uint32_t Ahb1 = Frequency;
	static constexpr uint32_t Ahb2 = Frequency;
	static constexpr uint32_t Apb = Frequency;

	// Usart 
	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;
	static constexpr uint32_t Usart3 = Apb;
	static constexpr uint32_t Usart4 = Apb;
	static constexpr uint32_t Usart5 = Apb;
	static constexpr uint32_t Usart6 = Apb;

	// Spi
	static constexpr int Spi2 = Frequency;

	static bool inline
	enable()
	{
		// enable internal 8 MHz HSI RC clock
		ClockControl::enableInternalClock();
		// (internal clock / 2) * 12 = 48MHz
		ClockControl::enablePll(ClockControl::PllSource::InternalClock, 12, 1);
		// set flash latency for 48MHz
		ClockControl::setFlashLatency(Frequency);
		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		ClockControl::setApbPrescaler(ClockControl::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(4000.f / (Frequency / 1000000));

		return true;
	}
};

using LedB = GpioOutputB4;
using LedY = GpioOutputB5;

using Rs = GpioOutputA8;
using Cs = GpioOutputB12;
using Sck = GpioOutputB13;
using Mosi = GpioOutputB15;
using Miso = GpioInputB14;
using Intr = GpioInputA11;

using LoraSpi = SpiMaster2;

using BluetoothDevice = xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull >; 

using LoggerDevice = xpcc::IODeviceWrapper< Usart3, xpcc::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	GpioOutputB10::connect(Usart3::Tx);
    GpioInputB11::connect(Usart3::Rx, Gpio::InputType::PullUp);
	Usart3::initialize<Board::systemClock, xpcc::Uart::Baudrate::B115200>(12);

	GpioOutputA2::connect(Usart2::Tx);
    GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, xpcc::Uart::Baudrate::B9600>(12);

	GpioOutputA9::connect(Usart1::Tx);
    GpioInputA10::connect(Usart1::Rx, Gpio::InputType::PullUp);
	Usart1::initialize<Board::systemClock, xpcc::Uart::Baudrate::B9600>(12);

}

}