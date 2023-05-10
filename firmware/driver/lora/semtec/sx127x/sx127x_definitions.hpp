#ifndef SX127X_DEFINITIONS_HPP
#define SX127X_DEFINITIONS_HPP

#include <stdint.h>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/math/utils/bit_constants.hpp>

namespace modm
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
    MODM_FLAGS8(RegAccess)

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
        RegPktSnrValue = 0x19,
        RegPktRssiValue = 0x1a,
        HopChannel = 0x1c,
        ModemConfig1 = 0x1d,
        ModemConfig2 = 0x1e,
        ModemConfig3 = 0x26,
        PayloadLength = 0x22,
        DioMapping1 = 0x40
    };
    typedef Configuration<RegAccess_t, Address, 0x7F> Address_t;

    // -- Common Registers -----------------------------------------------------

    // -- Operation Mode Register
    enum class
    RegOpMode : uint8_t
    {
        /// Sets device in either FSK/OOK mode (0) or LoRa mode (1)
        LongRangeMode = Bit7,
        // Bit6: AccessSharedReg
        // Bit5: Reserved
        // But4: Reserved
        /// Enables the high frequency driver (0) or the low frequency one (1)
        LowFrequencyModeOn = Bit3
    };
    MODM_FLAGS8(RegOpMode);

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
    typedef Configuration<RegOpMode_t, Mode, Bit0 | Bit1 | Bit2> Mode_t;

    // // -- RF Block Registers ---------------------------------------------------

    // // -- PA Config
    enum class
    RegPaConfig : uint8_t
    {
        /// Selects PA output pin, either RFO (0) or PA_BOOST (1)
        PaSelect = Bit7
    };
    MODM_FLAGS8(RegPaConfig);

    typedef Value<RegPaConfig_t, 3, 4>  MaxPower_t;
    typedef Value<RegPaConfig_t, 4, 0>  OutputPower_t;

    // -- PA Ramp
    enum class
    RegPaRamp : uint8_t
    {};
    MODM_FLAGS8(RegPaRamp)

    typedef Value<RegPaRamp_t, 4, 0> PaRamp_t;

    // -- LNA
    enum class
    RegLna : uint8_t
    {};
    MODM_FLAGS8(RegLna)

    typedef Value<RegLna_t, 3, 4> LnaGain_t;
    typedef Value<RegLna_t, 2, 2> LnaBoostLf_t;
    typedef Value<RegLna_t, 2, 0> LnaBoostHf_t;

    // // -- Lora Page Registers --------------------------------------------------

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
    MODM_FLAGS8(RegIrqFlagsMask)

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
    MODM_FLAGS8(RegIrqFlags)

    // -- Modem Config 1
    enum class
    RegModemConfig1 : uint8_t
    {
        /// Switches between implicit (1) or explicit (0) header mode
        ImplicitHeaderModeOn = Bit0
    };
    MODM_FLAGS8(RegModemConfig1)

    enum class
    SignalBandwidth : uint8_t
    {
        Fr7_8kHz   = 0,
        Fr10_4kHz  = Bit0,
        Fr15_6kHz  = Bit1,
        Fr20_8kHz  = Bit1 | Bit0,
        Fr31_25kHz = Bit2,
        Fr41_7kHz  = Bit2 | Bit0,
        Fr62_5kHz  = Bit2 | Bit1,
        Fr125kHz   = Bit2 | Bit1 | Bit0,
        Fr250kHz   = Bit3,
        Fr500kHz   = Bit3 | Bit0
    };
    typedef Configuration<RegModemConfig1_t, SignalBandwidth, ((Bit7 | Bit6 | Bit5 | Bit4) >> 4), 4> SignalBandwidth_t;

    enum class
    ErrorCodingRate : uint8_t
    {
        Cr4_5 = Bit0,
        Cr4_6 = Bit1,
        Cr4_7 = Bit1 | Bit0,
        Cr4_8 = Bit2
    };
    typedef Configuration<RegModemConfig1_t, ErrorCodingRate, ((Bit3 | Bit2 | Bit1) >> 1), 1> ErrorCodingRate_t;

    // -- Modem Config 2
    enum class
    RegModemConfig2 : uint8_t
    {
        /// Set either single package mode (0) or continuous mode (1)
        TxContinuousMode = Bit3,

        /// Enable (1) or disable (0) CRC check on payload
        RxPayloadCrcOn = Bit2
    };
    MODM_FLAGS8(RegModemConfig2)

    enum class
    SpreadingFactor : uint8_t
    {
        SF6  = 0x06,
        SF7  = 0x07,
        SF8  = 0x08,
        SF9  = 0x09,
        SF10 = 0x0a,
        SF11 = 0x0b,
        SF12 = 0x0c
    };
    typedef Configuration<RegModemConfig2_t, SpreadingFactor, ((Bit7 | Bit6 | Bit5 | Bit4) >> 4), 4> SpreadingFactor_t;
    typedef Value<RegModemConfig2_t, 2, 0>  SymbTimeoutMsb_t;

    // -- ModemConfig3
    enum class
    RegModemConfig3 : uint8_t
    {
        LowDataRateOptimize = Bit3,
        AgcAutoOn = Bit2
    };
    MODM_FLAGS8(RegModemConfig3)

    // -- Dio Mapping 1
    enum class
    RegDioMapping1 : uint8_t
    {};
    MODM_FLAGS8(RegDioMapping1)

    typedef Value<RegDioMapping1_t, 2, 6> Dio0Mapping_t;
};

}

#endif
