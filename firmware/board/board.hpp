// ----------------------------------------------------------------------------
/* Copyright (c) 2021, Lucas Moesch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef RLD_NODE_BOARD_HPP
#define RLD_NODE_BOARD_HPP

// #include <modm/io.hpp>
#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

namespace Board
{

using namespace modm::literals;

struct SystemClock
{
	// core and bus frequencys
	static constexpr uint32_t Frequency = 48_MHz;
    static constexpr uint32_t Hsi = 8_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	// Usart
	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;

    //Spi
    static constexpr uint32_t Spi2   = Apb;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock(); // 8 MHz
        const Rcc::PllFactors pllFactors{
			.pllMul = 12,
			.pllPrediv = 2
		};
        Rcc::enablePll(Rcc::PllSource::HsiDiv2, pllFactors);
		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
        Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

namespace lora {
	using Rst = GpioOutputA8;
	using D0 = GpioInputA3;

	using Nss = GpioOutputA4;
	using Sck = GpioOutputB13;
	using Miso = GpioInputB14;
	using Mosi = GpioOutputB15;

	using Spi = SpiMaster2;
}

namespace gps
{
	using Rx = GpioInputA10;
	using Tx = GpioOutputA9;

	using Uart = Usart1;
}

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	lora::Nss::setOutput();
	lora::Rst::setOutput();

	lora::Nss::set();
	lora::Rst::set();

	lora::Spi::connect<lora::Sck::Sck, lora::Mosi::Mosi, lora::Miso::Miso>();
	lora::Spi::initialize<SystemClock, 6000000ul>();

	// rpi::Uart::connect<rpi::Tx::Tx, rpi::Rx::Rx>();
	// rpi::Uart::initialize<SystemClock, 9600_Bd>();
}

}

#include "board_impl.hpp"
#endif