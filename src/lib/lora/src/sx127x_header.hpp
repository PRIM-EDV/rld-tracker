// ----------------------------------------------------------------------------
/* Copyright (c) 2018, Lucas Moesch
 * All Rights Reserved.
 *
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/interface/spi_device.hpp>
#include <xpcc/processing.hpp>

namespace xpcc
{

struct sx127x
{
public:
    enum class
    RegAccess : uint8_t
    {
        /// Defines read (0) or write (1) access
        wnr = Bit7
    };
    XPCC_FLAGS8(RegAccess)

    enum class 
    Address : uint8_t
    {
        Fifo = 0x00,

        // -- Common Registers -------------------------------------------------
        
        OpMode = 0x01,
        FrMsb = 0x06,
        FrMid = 0x07,
        FrLsb = 0x08,

        // -- RF Block Registers -----------------------------------------------

        PaConfig = 0x09,
        Lna = 0x0c,

        // -- LoRa Page Registers ----------------------------------------------

        FifoAddrPtr = 0x0d,
        FifoTxBaseAddr = 0x0e,
        FifoRxBaseAddr = 0x0f,
        FifoRxCurrAddr = 0x10,
        IrqFlagsMask = 0x11,
        IrqFlags = 0x12,
        RxNbBytes = 0x13,
        HopChannel = 0x1c,
        ModemConfig1 = 0x1d,
        ModemConfig2 = 0x1e,
        PayloadLength = 0x22,
        DioMapping1 = 0x40
    };
    typedef xpcc::Configuration<RegAccess_t, Address, 0x7F> Address_t;

    // -- Common Registers -----------------------------------------------------

    // -- Operation Mode 
    enum class 
    RegOpMode : uint8_t
    {
        /// Sets device in either FSK/OOK mode (0) or LoRa mode (1) 
        LongRangeMode = Bit7, 

        // Bit6: AccessSharedReg
        // Bit5: Reserved
        // But4: Reserved

        /// Enables the high frequency driver (0) or the low frequency one (1)
        LowFrequencyModeOn = Bit3, 
    };
    XPCC_FLAGS8(RegOpMode)

    enum class 
    Mode : uint8_t
    {
        Sleep = 0,
        Standby = Bit0,
        FreqSynthTX = Bit1,
        Transmit = Bit1 | Bit0,
        FreqSynthRX = Bit2,
        RecvCont = Bit2 | Bit0,
        RecvSingle = Bit2 | Bit1,
        ChnActvDetect = Bit2 | Bit1 | Bit0 
    };
    typedef xpcc::Configuration<RegOpMode_t, Mode, 0b111> Mode_t;

    // --

    // -- RF Block Registers ---------------------------------------------------

    // -- PA Config 
    enum class
    RegPaConfig : uint8_t
    {
        /// Selects PA output pin, either RFO (0) or PA_BOOST (1)
        PaSelect = Bit7
    };
    XPCC_FLAGS8(RegPaConfig)

    typedef xpcc::Value<RegPaConfig_t, 3, 0x04>  MaxPower_t;
    typedef xpcc::Value<RegPaConfig_t, 4, 0x00>  OutputPower_t;

    // -- PA Ramp
    enum class
    RegPaRamp : uint8_t
    {};
    XPCC_FLAGS8(RegPaRamp)

    typedef xpcc::Value<RegPaRamp_t, 4, 0x00> PaRamp_t;

    // -- LNA
    enum class
    RegLna : uint8_t
    {};
    XPCC_FLAGS8(RegLna)

    typedef xpcc::Value<RegLna_t, 3, 0x05> LnaGain_t;
    typedef xpcc::Value<RegLna_t, 2, 0x03> LnaBoostLf_t;
    typedef xpcc::Value<RegLna_t, 2, 0x00> LnaBoostHf_t;

    // -- Lora Page Registers --------------------------------------------------

    // IRQ Flags Mask
    enum class
    RegIrqFlagsMask : uint8_t
    {
        RxTimeoutMask = Bit7,
        RxDoneMask = Bit6,
        PayloadCrcErrorMask = Bit5,
        ValidHeaderMask = Bit4,
        TxDoneMask = Bit3,
        CadDoneMask = Bit2,
        FhssChangeChannelMask = Bit1,
        CadDetectedMask = Bit0
    };
    XPCC_FLAGS8(RegIrqFlagsMask)

    // IRQ FLags
    enum class
    RegIrqFlags : uint8_t
    {
        RxTimeout = Bit7,
        RxDone = Bit6,
        PayloadCrcError = Bit5,
        ValidHeader = Bit4,
        TxDone = Bit3,
        CadDone = Bit2,
        FhssChangeChannel = Bit1,
        CadDetected = Bit0
    };
    XPCC_FLAGS8(RegIrqFlags)

    // -- Modem Config 1
    enum class
    RegModemConfig1 : uint8_t
    {
        /// Switches between implicit (1) or explicit (0) header mode
        ImplicitHeaderModeOn = Bit0
    };
    XPCC_FLAGS8(RegModemConfig1)

    enum class
    SignalBandwidth : uint8_t
    {
        Fr7_8kHz = 0,
        Fr10_4kHz = Bit0,
        Fr15_6kHz = Bit1,
        Fr20_8kHz = Bit1 | Bit0,
        Fr31_25kHz = Bit2,
        Fr41_7kHz = Bit2 | Bit0,
        Fr62_5kHz = Bit2 | Bit1,
        Fr125kHz = Bit2 | Bit1 | Bit0,
        Fr250kHz = Bit3,
        Fr500kHz = Bit3 | Bit0 
    };
    typedef xpcc::Configuration<RegModemConfig1_t, SignalBandwidth, 0b1111, 0x04> SignalBandwidth_t;

    enum class
    ErrorCodingRate : uint8_t
    {
        Cr4_5 = Bit0,
        Cr4_6 = Bit1,
        Cr4_7 = Bit1 | Bit0,
        Cr4_8 = Bit2
    };
    typedef xpcc::Configuration<RegModemConfig1_t, ErrorCodingRate, 0b111, 0x01> ErrorCodingRate_t;

    // -- Modem Config 2
    enum class
    RegModemConfig2 : uint8_t
    {
        /// Set either single package mode (0) or continuous mode (1)
        TxContinuousMode = Bit3,

        /// Enable (1) or disable (0) CRC check on payload
        RxPayloadCrcOn = Bit2
    };
    XPCC_FLAGS8(RegModemConfig2)

    enum class
    SpreadingFactor : uint8_t
    {
        SF6 = 0x06,
        SF7 = 0x07,
        SF8 = 0x08,
        SF9 = 0x09,
        SF10 = 0x0a,
        SF11 = 0x0b,
        SF12 = 0x0c
    };
    typedef xpcc::Configuration<RegModemConfig2_t, SpreadingFactor, 0b1111, 0x04> SpreadingFactor_t;
    typedef xpcc::Value<RegModemConfig2_t, 2, 0x00>  SymbTimeoutMsb_t;

    // -- Dio Mapping 1
    enum class
    RegDioMapping1 : uint8_t
    {};
    XPCC_FLAGS8(RegDioMapping1)

    typedef xpcc::Value<RegDioMapping1_t, 2, 0x06> Dio0Mapping_t;


    // -- Register list --------------------------------------------------------
    union Shared {
        uint8_t value;
        RegOpMode_t regOpMode;    
        RegPaConfig_t regPaConfig;
        RegLna_t regLna;
        RegIrqFlagsMask_t regIrqFlagsMask;
        RegIrqFlags_t regIrqFlags; 
        RegModemConfig1_t regModemConfig1;
        RegModemConfig2_t regModemConfig2;
        RegDioMapping1_t regDioMapping1;
    };
};

/**
 * Driver for SX1276/7/8 based FSK/OOK, LoRa modules.
 *
 * Nice description.
 *
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Lucas Moesch
 * @ingroup ???
 */
template <typename SpiMaster, typename Cs>
class SX127x : public sx127x, public xpcc::SpiDevice<SpiMaster>, protected xpcc::NestedResumable<3>
{
public:
	SX127x();

	/// 
	xpcc::ResumableResult<void>
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
    xpcc::ResumableResult<void>
    write(Address addr, uint8_t data);

    xpcc::ResumableResult<void>
    write(Address addr, uint8_t *data, uint8_t nbBytes);

    xpcc::ResumableResult<void>
    read(Address addr, uint8_t *data, uint8_t nbBytes);

    // -- Advanced I/O ---------------------------------------------------------
    xpcc::ResumableResult<void>
    setLora();

    //xpcc::ResumableResult<void>
    //setFSK();

    xpcc::ResumableResult<void>
    setLowFrequencyMode();

    xpcc::ResumableResult<void>
    setLnaGain(uint8_t gain);

    //xpcc::ResumableResult<void>
    //setHighFrequencyMode();

    xpcc::ResumableResult<void>
    setOperationMode(Mode mode);

    xpcc::ResumableResult<void>
    setCarrierFreq(uint8_t msb, uint8_t mid, uint8_t lsb);

    xpcc::ResumableResult<void>
    setPaBoost();

    xpcc::ResumableResult<void>
    setOutputPower(uint8_t power);

    xpcc::ResumableResult<void>
    setBandwidth(SignalBandwidth bw);

    xpcc::ResumableResult<void>
    setCodingRate(ErrorCodingRate cr);

    xpcc::ResumableResult<void>
    setSpreadingFactor(SpreadingFactor sf);

    xpcc::ResumableResult<void>
    setImplicitHeaderMode();

    xpcc::ResumableResult<void>
    setExplicitHeaderMode();

    xpcc::ResumableResult<void>
    setDio0Mapping(uint8_t map);

    xpcc::ResumableResult<void>
    enablePayloadCRC();

    xpcc::ResumableResult<void>
    setPayloadLength(uint8_t len);

    xpcc::ResumableResult<bool>
    getInterrupt(RegIrqFlags irq);

    // -- Send/Receive ---------------------------------------------------------
    xpcc::ResumableResult<void>
    getPayload(uint8_t *data, uint8_t nbBytes);

    xpcc::ResumableResult<void>
    sendPacket(uint8_t *data, uint8_t nbBytes);

private:
    RegAccess_t regAccess;
    Shared _shared = {0x00};
};
} // Namespace xpcc
