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

    

    void setBool(uint8_t* index, uint8_t location, bool val);
    bool getBool(const uint8_t* index, uint8_t location);
    

    template <typename T> void set(uint8_t* index, T val);
    
    template <typename T> T get(const uint8_t* index);
}

#endif