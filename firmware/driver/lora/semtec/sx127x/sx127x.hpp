#ifndef SX127X_HPP
#define SX127X_HPP

#include <modm/architecture/interface/spi_device.hpp>

#include "sx127x_definitions.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs>
class SX127x : public sx127x, public SpiDevice<SpiMaster>, protected NestedResumable<2>
{
public:
	SX127x();

    ///
	ResumableResult<void>
	initialize();

    // -- Basic I/O ------------------------------------------------------------

    /**
     *  Write access to specified SX127x register.
     *
     *  Writes exactly one byte of data to the register specified by it's
     *  address.
     *
     *  @param addr Address of the register to write to.
     *  @param data Databyte to write to the specified address.
     */
    ResumableResult<void>
    write(Address addr, uint8_t data);

    ResumableResult<void>
    write(Address addr, uint8_t *data, uint8_t nbBytes);

    ResumableResult<void>
    read(Address addr, uint8_t *data, uint8_t nbBytes);

    // -- Advanced I/O ---------------------------------------------------------
    ResumableResult<void>
    setLora();

    //xpcc::ResumableResult<void>
    //setFSK();

    ResumableResult<void>
    setLowFrequencyMode();

    ResumableResult<void>
    setLnaGain(uint8_t gain);

    ResumableResult<void>
    setLnaBoostHf();

    //xpcc::ResumableResult<void>
    //setHighFrequencyMode();
    ResumableResult<void>
    setAgcAutoOn();

    ResumableResult<void>
    setLowDataRateOptimize();

    ResumableResult<void>
    setOperationMode(Mode mode);

    ResumableResult<void>
    setCarrierFreq(uint8_t msb, uint8_t mid, uint8_t lsb);

    ResumableResult<void>
    setCarrierFreq(frequency_t freq);

    ResumableResult<void>
    setPaBoost();

    ResumableResult<void>
    setOutputPower(uint8_t power);

    ResumableResult<void>
    setBandwidth(SignalBandwidth bw);

    ResumableResult<void>
    setCodingRate(ErrorCodingRate cr);

    ResumableResult<void>
    setSpreadingFactor(SpreadingFactor sf);

    ResumableResult<void>
    setImplicitHeaderMode();

    ResumableResult<void>
    setExplicitHeaderMode();

    ResumableResult<void>
    setDio0Mapping(uint8_t map);

    ResumableResult<void>
    enablePayloadCRC();

    ResumableResult<void>
    setPayloadLength(uint8_t len);

    ResumableResult<bool>
    getInterrupt(RegIrqFlags irq);

    // -- Send/Receive ---------------------------------------------------------
    ResumableResult<void>
    getPayload(uint8_t *data, uint8_t nbBytes);

    ResumableResult<void>
    sendPacket(uint8_t *data, uint8_t nbBytes);

private:
    uint8_t value;
    uint8_t buffer[4];
    int32_t frequency;
    RegAccess_t regAccess;

    union Shadow {
        RegOpMode_t regOpMode;
        RegPaConfig_t regPaConfig;
        RegLna_t regLna;
        RegIrqFlagsMask_t regIrqFlagsMask;
        RegIrqFlags_t regIrqFlags;
        RegModemConfig1_t regModemConfig1;
        RegModemConfig2_t regModemConfig2;
        RegModemConfig3_t regModemConfig3;
        RegDioMapping1_t regDioMapping1;

        Shadow() {this->regOpMode.value = 0x00;}
    } shadow;
};
}

#include "sx127x_impl.hpp"

#endif