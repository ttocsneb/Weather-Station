#include <iostream>
#include <RF24/RF24.h>

using std::cout;
using std::endl;

#define CE_PIN 25
#define CS_PIN 0

const uint8_t pip[6] = "HELL0";

RF24 radio(CE_PIN, CS_PIN);

int main(int argc, char** argv) {
    cout << "Hello World!" << endl;
    radio.begin();

    return 0;
}