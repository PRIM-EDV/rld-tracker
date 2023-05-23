/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#ifndef GPS_THREAD_HPP
#define GPS_THREAD_HPP

#ifndef M_PI
    #define M_PI (double) 3.14159265358979323846
#endif

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "driver/gps/beitian/bn280.hpp"
#include "shared/shared.hpp"

typedef struct {
    double latitude;
    double longitude;
} GeoCoordinates;

using namespace modm;
using namespace Beitian;

template <typename Uart>
class GPSThread : public modm::pt::Protothread, private modm::NestedResumable<2>
{
public:
    GeoCoordinates position;

    void
    initialize()
    {
        setOrigin(52.382864, 11.818967);
    };

    bool
    run()
    {
        PT_BEGIN();

        while (true) {
            PT_CALL(gps.update());

            gps.getGeoCoordinates(position.latitude, position.longitude);

            shared::latitude = position.latitude;
            shared::longitude = position.longitude;
        };

        PT_END();
    };



private:
	uint8_t data[8];
    double scaler = 1;

    GeoCoordinates origin;

    BN280<Uart> gps;

    void 
	setOrigin(double latitude, double longitude)
	{
		origin.latitude = latitude;
		origin.longitude = longitude;

		scaler = cos(latitude * M_PI / 180) * 111300;
	};

	void 
	setPosition(double latitude, double longitude)
	{
	 double y = (origin.latitude - latitude) * 111300;
	 double x = (longitude - origin.longitude) * scaler;

	//  posCart.y = (uint16_t) y;
	//  posCart.x = (uint16_t) x;
	}


};

#endif