#ifndef MAIN
#define MAIN

#include <stdint.h>

//Enable sleep_for(), sleep_until() commands
#include <chrono>
#include <thread>
#include <time.h>
using std::this_thread::sleep_for;
using std::this_thread::sleep_until;
using namespace std::chrono_literals;
using std::chrono::system_clock;
typedef std::chrono::time_point<std::chrono::system_clock> time_point;
typedef std::chrono::high_resolution_clock Clock;

#include <string>
std::string date();
#define timeDiff(t1, t2) std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()

namespace global {
    /**
     * Initialize the global namespace
     */
    void begin();

    /**
     * Turn on or off the indicator light
     * 
     * @param value 
     */
    void light(bool value);
    /**
     * Change the state of the indicator light
     */
    void toggleLight();

    
    /**
     * Set a boolean value in a uint8_t array
     * 
     * @param index pointer to the byte that the bool is stored
     * @param location binary location of the bool (0-7)
     * @param val value to set
     */
    void setBool(uint8_t* index, uint8_t location, bool val);
    /**
     * get boolean value set from setBool()
     * 
     * @param index pointer to the byte that the bool is stored
     * @param location binary location of the bool (0-7)
     * 
     * @return value
     */
    bool getBool(const uint8_t* index, uint8_t location);
    
    /**
     * set a number to a byte array
     * 
     * @note Only `uint8_t` `uint16_t` and `uint32_t` are allowed
     * 
     * @param index pointer to location to set value
     * @param val value
     */
    template <typename T> void set(uint8_t* index, T val);
    /**
     * Get a number from a byte array
     * 
     * @note Only `uint8_t` `uint16_t` and `uint32_t` are allowed
     * 
     * @param index pointer to location to get value
     * 
     * @return value
     */    
    template <typename T> T get(const uint8_t* index);
}

#endif