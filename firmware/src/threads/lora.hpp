/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef LORA_THREAD_HPP
#define LORA_THREAD_HPP

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>

#include "board/board.hpp"
#include "driver/ai-thinker/ra_02.hpp"

using namespace modm;

template <typename SpiMaster, typename Cs, typename D0>
class LoraThread : public modm::pt::Protothread, private modm::NestedResumable<2>
{
public:
    void
    initialize()
    {
        RF_CALL_BLOCKING(modem.setLora());
        RF_CALL_BLOCKING(modem.setCarrierFreq(0x6f, 0x00, 0x12));
        RF_CALL_BLOCKING(modem.setPaBoost());
        RF_CALL_BLOCKING(modem.setAgcAutoOn());
        RF_CALL_BLOCKING(modem.setExplicitHeaderMode());
        RF_CALL_BLOCKING(modem.setSpreadingFactor(sx127x::SpreadingFactor::SF9));
        RF_CALL_BLOCKING(modem.setBandwidth(sx127x::SignalBandwidth::Fr125kHz));
        RF_CALL_BLOCKING(modem.setCodingRate(sx127x::ErrorCodingRate::Cr4_5));
        RF_CALL_BLOCKING(modem.enablePayloadCRC());
        RF_CALL_BLOCKING(modem.setDio0Mapping(0));

        RF_CALL_BLOCKING(modem.setPayloadLength(4));

        // // Set output power to 10 dBm (boost mode)
        RF_CALL_BLOCKING(modem.setOutputPower(0x08));
        RF_CALL_BLOCKING(modem.setOperationMode(sx127x::Mode::RecvCont));
    };

    bool
    run()
    {
        PT_BEGIN();

        while (1) {
            if(timeout.isExpired()){

            }
            if(PT_CALL(this->receiveMessage(this->data)))
            {
                this->setRldInfo(this->data);
            };
        };

        PT_END();
    };

    void
    setRldInfo(uint8_t* data)
    {

    };

    ResumableResult<uint8_t>
	receiveMessage(uint8_t* buffer)
    {
        RF_BEGIN();

        while(true) {
            RF_WAIT_UNTIL(D0::read());
            RF_CALL(modem.read(sx127x::Address::IrqFlags, status, 1));
            if(!(status[0] & (uint8_t) sx127x::RegIrqFlags::PayloadCrcError)) {
                RF_CALL(modem.getPayload(buffer, 4));
                RF_RETURN(4);
            }
            RF_CALL(modem.write(sx127x::Address::IrqFlags, 0xff));
        }

        RF_END_RETURN(0);
    };

    ResumableResult<uint8_t>
	sendMessage(uint8_t* buffer)
    {
        RF_BEGIN();
        RF_CALL(modem.setOperationMode(sx127x::Mode::Transmit));

        // build packet

        RF_CALL(modem.setPayloadLength(4));
		RF_CALL(modem.sendPacket(data, 4));

		PT_CALL(modem.setOperationMode(sx127x::Mode::RecvCont));
		timeout.restart(5s);

        RF_END_RETURN(0);
    };

private:
	uint8_t data[8];
    uint8_t status[1];

    ShortTimeout timeout;

    Ra02<SpiMaster, Cs> modem;
};

#endif