// ----------------------------------------------------------------------------
/* Copyright (c) 2018, Lucas Moesch
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/interface/uart.hpp>
#include <xpcc/processing.hpp>

namespace xpcc
{

struct gps
{
private:
    typedef struct {
        char sentence[82];
        uint8_t pos;
    }nmea_t;

    typedef struct {
        double value;
        char direction;
    }geo;

    typedef struct {
        uint32_t time;
        char status;
        geo latitude;
        geo longitude;
        float speed;
        float course;
        uint32_t date;
        float variation;
        char hemisphere;
        uint8_t checksum;
    }rmc_t;

    typedef struct {
        char mode1;
        char mode2;
        uint8_t satId[12];
        float pdop;
        float hdop;
        float vdop;
    }gsa_t;
};

template <typename uart>
class GPS : public gps, protected xpcc::NestedResumable<3>
{
public:
    GPS();

    bool
    update();

    bool
    getPosition(double &lat, double &lon);

private:
    nmea_t nmea;
    rmc_t rmc;

    void
    parseLine();

    void
    clearLine();

    void
    parseRMC();

    void 
    parseGSA();
};

} // Namespace xpcc
