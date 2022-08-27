// ----------------------------------------------------------------------------
/* Copyright (c) 2017, Lucas Mösch
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "string.h"
#include "gps_header.hpp"

namespace xpcc
{

template <typename uart>
GPS<uart>::GPS()
{};

template <typename uart>
bool
GPS<uart>::update()
{
    uint8_t c;

    while(uart::read(c))
    {
        if(c == '$' || nmea.pos > 81) nmea.pos = 0;

        nmea.sentence[nmea.pos] = (char) c;
        nmea.pos++;

        if(c == 0x0d)
        {
            parseLine();
            return true;
        }else{
            return false;
        }
    }
};

template <typename uart>
bool
GPS<uart>::getPosition(double &lat, double &lon)
{
    if(rmc.longitude.value && rmc.latitude.value)
    {
        lat = rmc.latitude.value;
        lon = rmc.longitude.value;

        return true;
    }

    return false;
};

template <typename uart>
void 
GPS<uart>::parseLine()
{
    // Check for RMC
    if(strncmp((nmea.sentence+3), "RMC", 3) == 0) parseRMC();

    //XPCC_LOG_INFO << nmea.sentence << xpcc::endl;
    clearLine();
};

template <typename uart>
void
GPS<uart>::clearLine(){
    uint8_t i = 0;

    for(i = 0; i < 82; i++)
    {
        nmea.sentence[i] = '\0';
    }
}

template <typename uart>
void
GPS<uart>::parseRMC(){
    char *p;

    // Time
    p = strchr(nmea.sentence, (int)',') + 1;
    (*p == ',') ? (rmc.time = 0) : (rmc.time = atoi(p));

    // Status
    p = strchr(p+1, (int)',') + 1;
    rmc.status = *p;

    // Latitude
    p = strchr(p, (int)',') + 1;
    if(*p == ',')
    {
        rmc.latitude.value = 0;
    }else
    {
        double value = strtod(p, &p);
        uint8_t degree = (int) (value / 100);
        double minutes = (value - 100 * degree);

        rmc.latitude.value =  (double) (degree + (minutes / 60));
    }   
    
    // Lat orientation
    p = strchr(p, (int)',') + 1;
    (*p == ',') ? (rmc.latitude.direction = 'X') : (rmc.latitude.direction =  *p);

    // Longitude
    p = strchr(p, (int)',') + 1;
    if(*p == ',')
    {
        rmc.longitude.value = 0;
    }else
    {
        double value = strtod(p, &p);
        uint8_t degree = (int) (value / 100);
        double minutes = (value - (100 * degree));

        rmc.longitude.value =  (double) (degree + (minutes / 60));
    }   
    
    // Longitude orientation
    p = strchr(p, (int)',') + 1;
    (*p == ',') ? (rmc.longitude.direction = 'X') : (rmc.longitude.direction =  *p);

    // Speed over ground
    p = strchr(p, (int)',') + 1;
    (*p == ',') ? (rmc.speed = 0) : (rmc.speed =  strtof(p, &p));

    // Course
    p = strchr(p, (int)',') + 1;
    (*p == ',') ? (rmc.course = 0) : (rmc.course =  strtof(p, &p));

    // Date stamp
    p = strchr(p+1, (int)',') + 1;
    (*p == ',') ? (rmc.date = 0) : (rmc.date = atoi(p));

    // Variation
    p = strchr(p+1, (int)',');

    // ???
    p = strchr(p+1, (int)',');

    // checksum
    p = strchr(p+1, (int)',');
}

}