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
	static constexpr uint32_t Usart3 = Apb;

    //Spi
    static constexpr uint32_t Spi2   = Apb;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock(); // 8 MHz
        // (internal clock / 2) * 12 = 48MHz
        const Rcc::PllFactors pllFactors{
			.pllMul = 12,
			.pllPrediv = 2
		};
        Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
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

namespace led {
    using blue = GpioOutputB4;
    using yellow = GpioOutputB5;
}

namespace bluetooth {
    using Rx = GpioInputA3;
    using Tx = GpioOutputA2;

    using Uart = Usart2;
}

namespace usb {
    using Rx = GpioInputB11;
    using Tx = GpioOutputB10;

    using Uart = Usart3;
}

namespace lora {
	using Rst = GpioOutputA8;
	using D0 = GpioInputA11;

	using Nss = GpioOutputB12;
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

    bluetooth::Uart::connect<bluetooth::Tx::Tx, bluetooth::Rx::Rx>();
    bluetooth::Uart::initialize<SystemClock, 9600_Bd>();

    gps::Uart::connect<gps::Tx::Tx, gps::Rx::Rx>();
	gps::Uart::initialize<SystemClock, 9600_Bd>();

	usb::Uart::connect<usb::Tx::Tx, usb::Rx::Rx>();
	usb::Uart::initialize<SystemClock, 9600_Bd>();

    led::blue::setOutput();
    led::yellow::setOutput();

    led::blue::set();
    led::yellow::set();
}

}

#include "board_impl.hpp"
#endif