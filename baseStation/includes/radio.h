#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>
#include <cstddef>

#include "commands.h"
#include "main.h"

#define PACKET_SIZE 32


namespace radio {

    /**
     * Initialize the radio namespace
     */
    void begin();

    /**
     * Update the raw weather data with the station.
     * 
     * 
     * @return time in ms that the loop should be adjusted by.
     * @return -1 if failed to get packet.
     * 
     * @note should only be called once every 30 seconds
     */
    int update();

    
}



#endif