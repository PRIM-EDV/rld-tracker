// ----------------------------------------------------------------------------
/* Copyright (c) 2021, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

// #ifndef MODM_SX127X_HPP
// #   error "Don't include this file directly, use 'sx1276.hpp' instead!"
// #endif

#include "sx127x.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs>
SX127x<SpiMaster, Cs>::SX127x()
{

}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::initialize()
{
    RF_BEGIN();

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::write(Address addr, uint8_t data)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    // for write access a '1' is followed by the address
    regAccess.set(RegAccess::wnr);
    Address_t::set(regAccess, addr);

    SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);

    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(data));

	if (this->releaseMaster()) {
        Cs::set();
    }

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::write(Address addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    // for write access a '1' is followed by the address
    regAccess.set(RegAccess::wnr);
    Address_t::set(regAccess, addr);

    SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);

    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(data, nullptr, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::read(Address addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);

    regAccess.reset(RegAccess::wnr);
    Address_t::set(regAccess, addr);

    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(nullptr, data, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLora()
{
    RF_BEGIN();

    /// Put module into sleep mode in order to set LoRa Mode
    RF_CALL(read(Address::OpMode, &((shadow.regOpMode).value), 1));

    Mode_t::set(shadow.regOpMode, Mode::Sleep);

    RF_CALL(write(Address::OpMode, shadow.regOpMode.value));

    /// Set operation mode to LoRa mode
    shadow.regOpMode.set(RegOpMode::LongRangeMode);

    RF_CALL(write(Address::OpMode, shadow.regOpMode.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLowFrequencyMode()
{
    RF_BEGIN();

    // Read current configuration and set LowFrequencyMode to 1
    RF_CALL(read(Address::OpMode, &((shadow.regOpMode).value), 1));

    shadow.regOpMode.set(RegOpMode::LowFrequencyModeOn);

    RF_CALL(write(Address::OpMode, shadow.regOpMode.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setOperationMode(Mode mode)
{
    RF_BEGIN();

    // Read current configuration and set LowFrequencyMode to 1
    RF_CALL(read(Address::OpMode, &((shadow.regOpMode).value), 1));

    Mode_t::set(shadow.regOpMode, mode);

    RF_CALL(write(Address::OpMode, shadow.regOpMode.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setCarrierFreq(uint8_t msb, uint8_t mid, uint8_t lsb)
{
    RF_BEGIN();

    // Read current configuration and set operation mode to 'sleep'
    RF_CALL(read(Address::OpMode, &((shadow.regOpMode).value), 1));

    Mode_t::set(shadow.regOpMode, Mode::Standby);

    RF_CALL(write(Address::OpMode, shadow.regOpMode.value));

    // write the three frequency bytes (MSB->LSB)
    RF_CALL(write(Address::FrMsb, msb));
    RF_CALL(write(Address::FrMid, mid));
    RF_CALL(write(Address::FrLsb, lsb));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setCarrierFreq(frequency_t freq)
{
    RF_BEGIN();

    // Read current configuration and set operation mode to 'sleep'
    RF_CALL(read(Address::OpMode, &((shadow.regOpMode).value), 1));

    Mode_t::set(shadow.regOpMode, Mode::Standby);

    RF_CALL(write(Address::OpMode, shadow.regOpMode.value));

    frequency = static_cast<uint32_t>(freq * (static_cast<float>(1<<19) / static_cast<float>(32_MHz)));
    buffer[0] = static_cast<uint8_t>((frequency >> 16) & 0xFF);
	buffer[1] = static_cast<uint8_t>((frequency >> 8) & 0xFF);
	buffer[2] = static_cast<uint8_t>(frequency & 0xFF);

    // write the three frequency bytes (MSB->LSB)
    RF_CALL(write(Address::FrMsb, buffer, 3));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLnaGain(uint8_t gain)
{
    RF_BEGIN();

    // Read current configuration
    RF_CALL(read(Address::Lna, &((shadow.regLna).value), 1));

    LnaGain_t::set(shadow.regLna, gain);

    RF_CALL(write(Address::Lna, shadow.regLna.value));

    RF_END();
};

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLnaBoostHf()
{
    RF_BEGIN();

    // Read current configuration
    RF_CALL(read(Address::Lna, &((shadow.regLna).value), 1));

    LnaBoostHf_t::set(shadow.regLna, 0x03);

    RF_CALL(write(Address::Lna, shadow.regLna.value));

    RF_END();
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setAgcAutoOn()
{
    RF_BEGIN();
    // Read current configuration
    RF_CALL(read(Address::ModemConfig3, &((shadow.regModemConfig3).value), 1));

    shadow.regModemConfig3.set(RegModemConfig3::AgcAutoOn);

    RF_CALL(write(Address::ModemConfig3, shadow.regModemConfig3.value));

    RF_END();
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLowDataRateOptimize()
{
    RF_BEGIN();
    // Read current configuration
    RF_CALL(read(Address::ModemConfig3, &((shadow.regModemConfig3).value), 1));

    shadow.regModemConfig3.set(RegModemConfig3::LowDataRateOptimize);

    RF_CALL(write(Address::ModemConfig3, shadow.regModemConfig3.value));

    RF_END();
}


// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setPaBoost()
{
    RF_BEGIN();

    // Read current configuration
    RF_CALL(read(Address::PaConfig, &((shadow.regPaConfig).value), 1));

    shadow.regPaConfig.set(RegPaConfig::PaSelect);

    RF_CALL(write(Address::PaConfig, shadow.regPaConfig.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setOutputPower(uint8_t power)
{
    RF_BEGIN();

    // Read current configuration and set operation mode to 'standby'
    RF_CALL(read(Address::PaConfig, &((shadow.regPaConfig).value), 1));

    OutputPower_t::set(shadow.regPaConfig, power);

    RF_CALL(write(Address::PaConfig, shadow.regPaConfig.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setBandwidth(SignalBandwidth bw)
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((shadow.regModemConfig1).value), 1));

    SignalBandwidth_t::set(shadow.regModemConfig1, bw);

    RF_CALL(write(Address::ModemConfig1, shadow.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setCodingRate(ErrorCodingRate cr)
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((shadow.regModemConfig1).value), 1));

    ErrorCodingRate_t::set(shadow.regModemConfig1, cr);

    RF_CALL(write(Address::ModemConfig1, shadow.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setSpreadingFactor(SpreadingFactor sf)
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig2, &((shadow.regModemConfig2).value), 1));

    SpreadingFactor_t::set(shadow.regModemConfig2, sf);

    RF_CALL(write(Address::ModemConfig2, shadow.regModemConfig2.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setImplicitHeaderMode()
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((shadow.regModemConfig1).value), 1));

    shadow.regModemConfig1.set(RegModemConfig1::ImplicitHeaderModeOn);

    RF_CALL(write(Address::ModemConfig1, shadow.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setExplicitHeaderMode()
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((shadow.regModemConfig1).value), 1));

    shadow.regModemConfig1.reset(RegModemConfig1::ImplicitHeaderModeOn);

    RF_CALL(write(Address::ModemConfig1, shadow.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setDio0Mapping(uint8_t map)
{
    RF_BEGIN();

    RF_CALL(read(Address::DioMapping1, &((shadow.regDioMapping1).value), 1));

    Dio0Mapping_t::set(shadow.regDioMapping1, map);

    RF_CALL(write(Address::DioMapping1, shadow.regDioMapping1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::enablePayloadCRC()
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig2, &((shadow.regModemConfig2).value), 1));

    shadow.regModemConfig2.set(RegModemConfig2::RxPayloadCrcOn);

    RF_CALL(write(Address::ModemConfig2, shadow.regModemConfig2.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setPayloadLength(uint8_t len)
{
    RF_BEGIN();

    RF_CALL(write(Address::PayloadLength, len));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<bool>
SX127x<SpiMaster, Cs>::getInterrupt(RegIrqFlags irq)
{
    RF_BEGIN();

    RF_CALL(read(Address::IrqFlags, &((shadow.regIrqFlags).value), 1));

    RF_END_RETURN(shadow.regIrqFlags & irq);
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::getPayload(uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    // Clear RxDone interrupt flag
    RF_CALL(write(Address::IrqFlags, (uint8_t) RegIrqFlags::RxDone));

    // Set Fifo address pointer to payload address
    RF_CALL(read(Address::FifoRxCurrAddr, &(value), 1));
    RF_CALL(write(Address::FifoAddrPtr, value));

    // Read payload
    RF_CALL(read(Address::Fifo, data, nbBytes));

    // Reset Fifo address pointer
    RF_CALL(read(Address::FifoRxBaseAddr, &(value), 1));
    RF_CALL(write(Address::FifoAddrPtr, value));

    // Todo: Check for incoming package before resetting the Addr pointer

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::sendPacket(uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    // Clear TxDone interrupt flag
    RF_CALL(write(Address::IrqFlags, (uint8_t) RegIrqFlags::TxDone));

    // Set Fifo address pointer to base address
    RF_CALL(read(Address::FifoTxBaseAddr, &(value), 1));
    RF_CALL(write(Address::FifoAddrPtr, value));

    // Write payload to Fifo
    RF_CALL(write(Address::Fifo, data, nbBytes));

    // Send the package
    RF_CALL(setOperationMode(Mode::Transmit));

    RF_END();
};

} // end namespace modm