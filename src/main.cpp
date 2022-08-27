// ----------------------------------------------------------------------------
/* Copyright (c) 2018, Lucas Moesch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
 
#include "board.hpp"
#include "lib/lora/src/sx127x.hpp"
#include "lib/gps/src/gps.hpp"

#define TRACKERID 19
#define TRACKERNAME "RLD-Kammerjäger"

using namespace Board;

BluetoothDevice bluetoothDevice;
xpcc::IOStream BLUETOOTH(bluetoothDevice);

// -- GPS Thread ---------------------------------------------------------------

using xpcc::GPS;
class GPSThread : public xpcc::pt::Protothread
{
	typedef struct {
        double lon;
		double lat;
    }posGps_t;

	typedef struct {
        double x;
		double y;
    }posCart_t;

	posGps_t origin, posGps;
	posCart_t posCart;
	double scaler;


public:
	void
	init()
	{
		setOrigin(52.382864, 11.818967);
	
	}
	bool
	update()
	{
		PT_BEGIN();

		while(1)
		{
			PT_WAIT_UNTIL(gps.update());

			if (gps.getPosition(posGps.lat, posGps.lon)){
				setPosition(posGps.lat, posGps.lon);
			}
			else
			{

			}
		}

		PT_END();
	}

	bool
	getPosition(uint16_t &px, uint16_t &py)
	{
		if (posCart.x != 0 && posCart.y !=0){
			LedY::setOutput(xpcc::Gpio::Low);
		}
		px = posCart.x;
		py = posCart.y;
	}

	void 
	setOrigin(double lat, double lon)
	{
		origin.lat = lat;
		origin.lon = lon;

		scaler = cos(lat * M_PI / 180) * 111300;
	}

	void 
	setPosition(double lat, double lon)
	{
	 double y = (origin.lat - lat) * 111300;
	 double x = (lon - origin.lon) * scaler;

	 posCart.y = (uint16_t) y;
	 posCart.x = (uint16_t) x;
	}

private:
	xpcc::GPS<Usart1> gps;
} gpsThread;

// -- LoRa Thread -------------------------------------------------------------

using xpcc::sx127x;
class LoraThread : public xpcc::pt::Protothread
{
public:
	uint8_t data[8];
	sx127x::RegModemConfig1_t config;

	uint8_t sf = 0;
	uint8_t bw = 0;
	uint8_t pw = 0;
	
	///
	void
	init()
	{
		// -- Configure the SPI and Reset/Interrupt pins -----------------------
		XPCC_LOG_INFO << "Initializing LoRa SPI" << xpcc::endl;

		Sck::connect(LoraSpi::Sck);
		Mosi::connect(LoraSpi::Mosi);
		Miso::connect(LoraSpi::Miso);

		Cs::setOutput(xpcc::Gpio::High);
		Rs::setOutput(xpcc::Gpio::High);

		LoraSpi::initialize<Board::systemClock, 6000000ul>();

		// -- Set user specific config -----------------------------------------
		XPCC_LOG_INFO << "Configuring SX1278" << xpcc::endl;
		
		RF_CALL_BLOCKING(sx1278.setLora());

		// Set frequency to 433.05 MHz (0x6c4345 * 61.035)
		RF_CALL_BLOCKING(sx1278.setCarrierFreq(0x6f, 0x00, 0x12));
		RF_CALL_BLOCKING(sx1278.read(sx127x::Address::FrMsb, data, 3));
		XPCC_LOG_INFO << "Frequency:" << data[0] << data[1] << data[2] << xpcc::endl; 

		//RF_CALL_BLOCKING(sx1278.setLowFrequencyMode());
		//RF_CALL_BLOCKING(sx1278.setImplicitHeaderMode());
		RF_CALL_BLOCKING(sx1278.setExplicitHeaderMode());
		RF_CALL_BLOCKING(sx1278.setPaBoost());
		RF_CALL_BLOCKING(sx1278.enablePayloadCRC());
		RF_CALL_BLOCKING(sx1278.setSpreadingFactor(sx127x::SpreadingFactor::SF9));
		RF_CALL_BLOCKING(sx1278.setBandwidth(sx127x::SignalBandwidth::Fr125kHz));
		RF_CALL_BLOCKING(sx1278.setCodingRate(sx127x::ErrorCodingRate::Cr4_5));
		RF_CALL_BLOCKING(sx1278.setDio0Mapping(0));

		RF_CALL_BLOCKING(sx1278.setPayloadLength(4));

		// Set output power to 10 dBm (boost mode)
		RF_CALL_BLOCKING(sx1278.setOutputPower(0x08));

		// set Timer
		timeout.restart(5000);
	};

	bool
	update()
	{
		PT_BEGIN();

		while(1)
		{
			if(timeout.isExpired() && !Intr::read()){
				//XPCC_LOG_INFO << "Switched to Send mode" << xpcc::endl;
				PT_CALL(sx1278.setOperationMode(sx127x::Mode::Transmit));

				// Maybe set SF, POWER, ETC

				buildPacket();

				PT_CALL(sx1278.setPayloadLength(4));
				PT_CALL(sx1278.sendPacket(data, 4));

				//Go to cont. receive mode
				//XPCC_LOG_INFO << "Switched to Receive mode" << xpcc::endl;	
				PT_CALL(sx1278.setOperationMode(sx127x::Mode::RecvCont));
				timeout.restart(5000);

				BLUETOOTH <<  0 << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << xpcc::endl;

			}
			else
			{
				PT_WAIT_UNTIL(Intr::read() || timeout.isExpired());
			
				if(Intr::read())
				{
					PT_CALL(sx1278.read(sx127x::Address::IrqFlags, data, 1));

					if(data[0] & (uint8_t) sx127x::RegIrqFlags::PayloadCrcError){
						//XPCC_LOG_INFO <<  "Irq:" << data[0] <<  xpcc::endl;
						//PT_CALL(sx1278.getPayload(data, 4))
					}else{
						PT_CALL(sx1278.getPayload(data, 4));
						XPCC_LOG_INFO << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << xpcc::endl;
						BLUETOOTH << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << xpcc::endl;			
					}

					PT_CALL(sx1278.write(sx127x::Address::IrqFlags, 0xff));
				}	
			} 
		}

		PT_END();
	};

private:
	xpcc::ShortTimeout timeout;
	xpcc::SX127x<LoraSpi, Cs> sx1278;

	void
	buildPacket(){
		uint16_t px;
		uint16_t py;

		gpsThread.getPosition(px, py);

		//XPCC_LOG_INFO << px << ":" << py << xpcc::endl;

		data[0] = TRACKERID;
		data[1] = (px >> 6) & 0x0F;
		data[2] = (px << 2) | ((py >>8) & 0x03);
		data[3] = (py) & 0xFF;
	};
} loraThread; 

// -- Control Thread -----------------------------------------------------------

class ControlThread : public xpcc::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		while(1)
		{
			PT_WAIT_UNTIL(lg.read(c));
			XPCC_LOG_INFO << "Byte received" << xpcc::endl;
		}

		PT_END();
	}

private:
	LoggerDevice lg;
	char c;
} controlThread;

int
main()
{
	Board::initialize();

	LedB::setOutput(xpcc::Gpio::High);
	LedY::setOutput(xpcc::Gpio::High);


	gpsThread.init();
	loraThread.init();

	//XPCC_LOG_INFO << "Starting Application." << xpcc::endl;
	//BLUETOOTH << "AT+NAME" << TRACKERNAME << xpcc::endl;

	while (1)
	{
		//controlThread.update();
		loraThread.update();
		gpsThread.update();
	}

	return 0;
}
