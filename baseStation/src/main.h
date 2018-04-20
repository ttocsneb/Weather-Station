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
    void begin();

    void light(bool value);
    void toggleLight();


    void setB(uint8_t* index, uint8_t location, bool val);
    void set8(uint8_t* index, uint8_t val);
    void set16(uint8_t* index, uint16_t val);
    void set32(uint8_t* index, uint32_t val);

    bool getB(uint8_t* index, uint8_t location);
    uint8_t get8(uint8_t* index);
    uint16_t get16(uint8_t* index);
    uint32_t get32(uint8_t* index);
}

#endif