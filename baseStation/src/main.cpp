#include <iostream>


#include "main.h"
#include "radio.h"

#include <wiringPi.h>

using std::cout;
using std::endl;

int main(int argc, char** argv) {
    wiringPiSetup();

    pinMode(27, OUTPUT);

    radio::begin();

    for(;;) {
	cout << "LIGHT ON" << endl;
	digitalWrite(27, HIGH); delay(500);
	cout << "LIGHT OFF" << endl;
	digitalWrite(27, LOW); delay(500);
    }

    return 0;
}
